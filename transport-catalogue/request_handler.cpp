#include "request_handler.h"

namespace transport_catalogue::output {

    RequestHander::RequestHander(backend::TransportCatalogue& tc)
        : transport_catalogue_(&tc) {

    }

    json::Dict RequestHander::GetBusQuery(const std::string& name, int id) {
        json::Dict result;
        result["request_id"] = json::Node(id);
        detail::Bus* bus = transport_catalogue_->GetBus(name);
        if(bus == nullptr) {
            using namespace std::literals;
            result["error_message"] = json::Node("not found"s);
        } else {
            detail::BusInfo bus_info = transport_catalogue_->GetBusInfo(bus);
            result["curvature"] = json::Node(bus_info.curvature);
            result["route_length"] = json::Node(bus_info.length);
            result["stop_count"] = json::Node(bus_info.stop_count);
            result["unique_stop_count"] = json::Node(bus_info.unique_stop_count);
        }
        return result;
    }

    json::Dict RequestHander::GetStopQuery(const std::string& name, int id) {
        json::Dict result;
        result["request_id"] = json::Node(id);
        detail::Stop* stop = transport_catalogue_->GetStop(name);
        if(stop == nullptr) {
            using namespace std::literals;
            result["error_message"] = json::Node("not found"s);
        } else {
            detail::StopInfo stop_info = transport_catalogue_->GetStopInfo(stop);
            json::Array buses;
            for(const std::string& bus : stop_info.buses) {
                buses.push_back(json::Node(bus));
            }
            result["buses"] = std::move(buses);
        }
        return result;
    }

    json::Dict RequestHander::GetMap(int id, render::MapRenderer& renderer) {
        json::Dict result;
        result["request_id"] = json::Node(id);
        std::stringstream strm;
        renderer.Print(strm);
        result["map"] = strm.str();
        return result;
    }

    void RequestHander::PrintRequests(std::ostream& os, input::JsonReader& requests, render::MapRenderer& renderer) {
        json::Array output;
        detail::OutputQuery temp;
        while(requests.GetQuery(temp)) {
            if(temp.type == "Bus") {
                output.push_back(json::Node(GetBusQuery(*temp.name, temp.id)));
            } else if(temp.type == "Stop") {
                output.push_back(json::Node(GetStopQuery(*temp.name, temp.id)));
            } else if(temp.type == "Map") {
                output.push_back(json::Node(GetMap(temp.id, renderer)));
            }
        }
        json::Print(json::Document(output), os);
    }

    std::vector<detail::BusCoordinates> RequestHander::GetCoordinates() {
        std::vector<detail::BusCoordinates> result;
        std::vector<detail::Bus*> buses = transport_catalogue_->GetAllBuses();
        for(detail::Bus* bus : buses) {
            std::vector<geo::Coordinates> coords;
            for(detail::Stop* stop : bus->stops) {
                coords.push_back(stop->location);
            }
            if(!bus->is_looped) {
                std::for_each(bus->stops.rbegin() + 1, bus->stops.rend(), [this, &coords](detail::Stop* stop){
                    coords.push_back(stop->location);
                });
            }
            result.push_back({bus, coords});
        }
        return result;
    }

    std::vector<detail::Stop*> RequestHander::GetStops() {
        return transport_catalogue_->GetAllStops();
    }
}

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */