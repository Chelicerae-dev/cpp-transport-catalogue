#include "json_reader.h"

namespace transport_catalogue::input {
    JsonReader::JsonReader(json::Document input, backend::TransportCatalogue& transport_catalogue) {
        json::Dict input_data = input.GetRoot().AsMap();
        json::Array input_requests = input_data.at("base_requests").AsArray();
        json::Array output_requests;
        if(input_data.count("stat_requests") != 0) output_requests = input_data.at("stat_requests").AsArray();
        std::for_each(input_requests.begin(), input_requests.end(), [this, &transport_catalogue](const json::Node& node){
            json::Dict query = node.AsMap();
            if(query.at("type").AsString() == "Stop") {
                ParseStop(transport_catalogue, &query);
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

        //Обрабатываем запросы на вывод
        if(output_requests.size() != 0) {
            for(const json::Node& node : output_requests) {
                ParseRequest(node.AsMap());
            }
        }

        //Сохраняем настройки рендера
        if(input_data.count("render_settings") != 0) ParseRenderSettings(input_data.at("render_settings").AsMap());
    }

    void JsonReader::ParseStop(backend::TransportCatalogue& transport_catalogue, json::Dict* node) {
        //получаем имя остановки
        std::string name = node->at("name").AsString();
        double longitude = node->at("longitude").AsDouble();
        double latitude = node->at("latitude").AsDouble();
        //сначала парсим расстояния если они есть
        if(node->count("road_distances") != 0) {
            json::Dict distances = node->at("road_distances").AsMap();
            ParseDistances(name, &distances);
        }
        //создаём остановку
        transport_catalogue.AddStop({std::move(name), {latitude, longitude}});
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
        if(node.count("bus_label_font_size") != 0) render_settings_.bus_label_font_size = node.at("bus_label_font_size").AsInt();
        if(node.count("stop_label_offset") != 0) render_settings_.stop_label_offset = {node.at("stop_label_offset").AsArray()[0].AsDouble(), node.at("stop_label_offset").AsArray()[1].AsDouble()};
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
                int r = temp[0].AsInt();
                int g = temp[1].AsInt();
                int b = temp[2].AsInt();
                double op = temp[3].AsDouble();
                return svg::Rgba(temp[0].AsInt(), temp[1].AsInt(), temp[2].AsInt(), temp[3].AsDouble());
            }
        }
    }

    const render::RenderSettings& JsonReader::GetRenderSettings() const {
        return render_settings_;
    }

}
