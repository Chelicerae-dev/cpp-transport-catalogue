#include "request_handler.h"

namespace transport_catalogue::output {

    json::Dict GetBusQuery(const std::string& name, int id, backend::TransportCatalogue& transport_catalogue) {
        json::Dict result;
        result["request_id"] = json::Node(id);
        detail::Bus* bus = transport_catalogue.GetBus(name);
        if(bus == nullptr) {
            using namespace std::literals;
            result["error_message"] = json::Node("not found"s);
        } else {
            detail::BusInfo bus_info = transport_catalogue.GetBusInfo(bus);
            result["curvature"] = json::Node(bus_info.curvature);
            result["route_length"] = json::Node(bus_info.length);
            result["stop_count"] = json::Node(bus_info.stop_count);
            result["unique_stop_count"] = json::Node(bus_info.unique_stop_count);
        }
        return result;
    }

    json::Dict GetStopQuery(const std::string& name, int id, backend::TransportCatalogue& transport_catalogue) {
        json::Dict result;
        result["request_id"] = json::Node(id);
        detail::Stop* stop = transport_catalogue.GetStop(name);
        if(stop == nullptr) {
            using namespace std::literals;
            result["error_message"] = json::Node("not found"s);
        } else {
            detail::StopInfo stop_info = transport_catalogue.GetStopInfo(stop);
            json::Array buses;
            for(const std::string& bus : stop_info.buses) {
                buses.push_back(json::Node(bus));
            }
            result["buses"] = std::move(buses);
        }
        return result;
    }

    void PrintRequests(std::ostream& os, input::JsonReader& requests, backend::TransportCatalogue& transport_catalogue) {
        json::Array output;
        detail::OutputQuery temp;
        while(requests.GetQuery(temp)) {
            if(temp.type == "Bus") {
                output.push_back(json::Node(GetBusQuery(temp.name, temp.id, transport_catalogue)));
            } else if(temp.type == "Stop") {
                output.push_back(json::Node(GetStopQuery(temp.name, temp.id, transport_catalogue)));
            }
        }
        json::Print(json::Document(output), os);
    }
}

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
