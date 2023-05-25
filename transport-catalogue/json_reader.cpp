#include "json_reader.h"

namespace transport_catalogue::input {
    JsonReader::JsonReader(json::Document input, backend::TransportCatalogue& transport_catalogue) {
        json::Dict input_data = input.GetRoot().AsMap();
        json::Array input_requests = input_data.at("base_requests").AsArray();
        json::Array output_requests = input_data.at("stat_requests").AsArray();
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

        std::for_each(output_requests.begin(), output_requests.end(), [this](const json::Node& node){
            ParseRequest(node.AsMap());
        });
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
        requests_query_.push_back({node.at("id").AsInt(), node.at("name").AsString(), node.at("type").AsString()});
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

}
