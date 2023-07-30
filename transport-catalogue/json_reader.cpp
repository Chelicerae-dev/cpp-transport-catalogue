#include "json_reader.h"

namespace transport_catalogue::input {
JsonReader::JsonReader(json::Document& input) {
    json::Dict input_data = input.GetRoot().AsDict();
    json::Array output_requests = input_data.at("stat_requests").AsArray();
    if(input_data.count("serialization_settings") != 0) ParseSerializationSettings(input_data.at("serialization_settings").AsDict());

    if(output_requests.size() != 0) {
        for(const json::Node& node : output_requests) {
            ParseRequest(node.AsDict());
        }
    }
}

    JsonReader::JsonReader(backend::TransportCatalogue& transport_catalogue, json::Document& input)
        : catalogue_(&transport_catalogue)
    {
        json::Dict input_data = input.GetRoot().AsDict();
        json::Array input_requests = input_data.at("base_requests").AsArray();
        if(input_data.count("serialization_settings") != 0) ParseSerializationSettings(input_data.at("serialization_settings").AsDict());

        std::for_each(input_requests.begin(), input_requests.end(), [this](const json::Node& node){
            json::Dict query = node.AsDict();
            if(query.at("type").AsString() == "Stop") {
                ParseStop(&query);
            } else {
                ParseBus(&query);
            }
        });

        //Добавляем дистанции
        std::for_each(stops_query_.begin(), stops_query_.end(), [&transport_catalogue](auto& stop) {
            detail::Stop* stop_ptr = transport_catalogue.GetStop(stop.name);
            transport_catalogue.AddStopDistances(stop_ptr, stop.distances);
        });

        //Добавляем автобусы в базу
        std::for_each(buses_query_.begin(), buses_query_.end(), [&transport_catalogue] (auto& this_bus) {
            transport_catalogue.AddBus(transport_catalogue.MakeBus(this_bus.name, this_bus.stops, this_bus.is_looped));
        });

        //Сохраняем настройки рендера
        if(input_data.count("render_settings") != 0) ParseRenderSettings(input_data.at("render_settings").AsDict());

        //устанавливаем настройки роутера в transport_catalogue
        if(input_data.count("routing_settings") != 0) {
            routing_settings_ = {static_cast<uint16_t>(input_data.at("routing_settings").AsDict().at("bus_wait_time").AsInt()), input_data.at("routing_settings").AsDict().at("bus_velocity").AsDouble()};
        }
    }

    json::Document JsonReader::ProcessRequests(std::function<detail::BusAnswer(const std::string&, int)> bus_proc,
                                                  std::function<detail::StopAnswer(const std::string&, int)> stop_proc,
                                                  std::function<detail::MapAnswer(int)> map_proc,
                                                  std::function<detail::RoutingAnswer(const std::string&, const std::string&, int)> routing_proc) {
        json::Array output;
        for(const auto& request : requests_query_) {
            if(request.type == "Bus") {
                detail::BusAnswer bus = bus_proc(request.name.value(), request.id);
                json::Builder result{};
                result.StartDict();
                result.Key("request_id").Value(bus.id);
                if(bus.exists) {
                    result.Key("curvature").Value(bus.bus_info->curvature);
                    result.Key("route_length").Value(bus.bus_info->length);
                    result.Key("stop_count").Value(bus.bus_info->stop_count);
                    result.Key("unique_stop_count").Value(bus.bus_info->unique_stop_count);
                } else {
                    result.Key("error_message").Value(bus.error_message);
                }
                result.EndDict();
                output.push_back(result.Build());
            } else if(request.type == "Stop") {
                detail::StopAnswer stop = stop_proc(request.name.value(), request.id);
                json::Builder result{};
                result.StartDict();
                result.Key("request_id").Value(stop.id);
                if(stop.exists) {
                    result.Key("buses").StartArray();
                    for(const std::string& bus : stop.stop_info->buses) {
                        result.Value(bus);
                    }
                    result.EndArray();

                } else {
                    using namespace std::literals;
                    result.Key("error_message").Value("not found"s);
                }
                result.EndDict();
                output.push_back(result.Build());
            } else if(request.type == "Map") {
                json::Builder result{};
                result.StartDict();
                detail::MapAnswer map = map_proc(request.id);
                result.Key("request_id").Value(map.id);
                result.Key("map").Value(map.map);
                result.EndDict();
                output.push_back(result.Build());
            } else if(request.type == "Route") {
                //готовим вывод элемента по заданному шаблону, не забыть убрать комментарии и поставить функцию, которую ещё предстоит написать
                json::Builder result{};
                result.StartDict();
                detail::RoutingAnswer route = routing_proc(request.from.value(), request.to.value(), request.id);
                result.Key("request_id").Value(route.id);
                if(route.exists) {
                    result.Key("total_time").Value(route.total_time.value());
                    result.Key("items").StartArray();
                    for(const auto& item : route.items.value()) {
                        result.StartDict();
                        result.Key("type");
                        if(item.type == detail::RouteItem::RouteItemType::WAIT) {
                            result.Value("Wait");
                            result.Key("stop_name").Value(std::string(item.name));
                        } else {
                            result.Value("Bus");
                            result.Key("bus").Value(std::string(item.name));
                            result.Key("span_count").Value(item.span_count.value());
                        }
                        result.Key("time").Value(item.time.value());
                        result.EndDict();
                    }
                    result.EndArray();
                } else {
                    using namespace std::literals;
                    result.Key("error_message").Value("not found"s);
                }
                result.EndDict();
                output.push_back(result.Build());
            }
        }
        return json::Document(output);
    }

    void JsonReader::ParseStop(json::Dict* node) {
        //получаем имя остановки
        std::string name = node->at("name").AsString();
        double longitude = 0.;
        double latitude = 0.;
        if(node->at("longitude").IsDouble()) {
            longitude = node->at("longitude").AsDouble();
        } else if(node->at("longitude").IsInt()) {
            longitude = node->at("longitude").AsInt() * 1.;
        }
        if(node->at("latitude").IsDouble()) {
            latitude = node->at("latitude").AsDouble();
        } else if(node->at("longitude").IsInt()) {
            latitude = node->at("latitude").AsInt() * 1.;
        }
        //сначала парсим расстояния если они есть
        if(node->count("road_distances") != 0) {
            json::Dict distances = node->at("road_distances").AsDict();
            ParseDistances(name, &distances);
        }
        //создаём остановку
        catalogue_->AddStop({std::move(name), {latitude, longitude}});
    }

    void JsonReader::ParseDistances(std::string_view name, json::Dict* node) {
        std::map<std::string, int> distances;
        //пишем во временную мапу
        for(const auto [key, val] : *node) {
            distances[key] = val.AsInt();
        }
        //добавляем в очередь
        stops_query_.push_back({std::string(name), std::move(distances)});
    }

    void JsonReader::ParseBus(json::Dict* node) {
        std::string name = node->at("name").AsString();
        bool is_looped = node->at("is_roundtrip").AsBool();
        std::vector<std::string> stops;
        json::Array stops_array = node->at("stops").AsArray();
        std::for_each(stops_array.begin(), stops_array.end(), [&stops](const json::Node& stop_node) {
           stops.push_back(stop_node.AsString());
        });
        buses_query_.push_back({std::string(name), stops, is_looped});
    }

    void JsonReader::ParseRequest(const json::Dict& node) {
        if(node.at("type").AsString() == "Map") {
            requests_query_.push_back({node.at("id").AsInt(), node.at("type").AsString()});
        } else if(node.at("type").AsString() == "Route") {
            requests_query_.push_back({node.at("id").AsInt(), node.at("type").AsString(), node.at("from").AsString(), node.at("to").AsString()});
        } else {
            requests_query_.push_back({node.at("id").AsInt(), node.at("type").AsString(), node.at("name").AsString()});
        }
    }

    bool JsonReader::GetQuery(detail::OutputQuery& output) {
        //возвращаем 0 и выходим если запросы кончились
        if(requests_query_.size() == 0) {
            return false;
        }
        output = requests_query_.front();
        requests_query_.pop_front();
        return true;
    }

    void JsonReader::ParseRenderSettings(const json::Dict& node) {

        if(node.count("width") != 0) render_settings_.width = node.at("width").AsDouble();
        if(node.count("height") != 0) render_settings_.height = node.at("height").AsDouble();
        if(node.count("padding") != 0) render_settings_.padding = node.at("padding").AsDouble();
        if(node.count("line_width") != 0) render_settings_.line_width = node.at("line_width").AsDouble();
        if(node.count("stop_radius") != 0) render_settings_.stop_radius = node.at("stop_radius").AsDouble();
        if(node.count("bus_label_offset") != 0) render_settings_.bus_label_offset = {node.at("bus_label_offset").AsArray()[0].AsDouble(), node.at("bus_label_offset").AsArray()[1].AsDouble()};
        if(node.count("bus_label_font_size") != 0) render_settings_.bus_label_font_size = node.at("bus_label_font_size").AsInt();
        if(node.count("stop_label_offset") != 0) render_settings_.stop_label_offset = {node.at("stop_label_offset").AsArray()[0].AsDouble(), node.at("stop_label_offset").AsArray()[1].AsDouble()};
        if(node.count("stop_label_font_size") != 0) render_settings_.stop_label_font_size = node.at("stop_label_font_size").AsInt();
        if(node.count("underlayer_color") != 0) render_settings_.underlayer_color = ParseSvgColor(node.at("underlayer_color"));
        if(node.count("underlayer_width") != 0) render_settings_.underlayer_width = node.at("underlayer_width").AsDouble();
        if(node.count("color_palette") != 0) {
            std::for_each(node.at("color_palette").AsArray().begin(), node.at("color_palette").AsArray().end(), [this](const json::Node& node){
                render_settings_.color_palette.push_back(ParseSvgColor(node));
            });
        }
    }

    svg::Color JsonReader::ParseSvgColor(const json::Node& node) {
        if(node.IsString()) {
            return node.AsString();
        } else {
            //выводим в переменную чтобы было удобнее
            json::Array temp = node.AsArray();
            if(temp.size() == 3) {
                return svg::Rgb(temp[0].AsInt(), temp[1].AsInt(), temp[2].AsInt());
            } else { //так же, как и выше
                return svg::Rgba(temp[0].AsInt(), temp[1].AsInt(), temp[2].AsInt(), temp[3].AsDouble());
            }
        }
    }

    void JsonReader::ParseSerializationSettings(const json::Dict& node) {
        if(node.count("file") != 0) serialization_settings_.file = node.at("file").AsString();
    }

    const render::RenderSettings& JsonReader::GetRenderSettings() const {
        return render_settings_;
    }

    const detail::RoutingSettings& JsonReader::GetRoutingSettings() const {
        return routing_settings_;
    }

    const detail::SerializationSettings& JsonReader::GetSerializationSettings() const {
        return serialization_settings_;
    }

}
