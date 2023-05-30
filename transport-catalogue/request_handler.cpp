#include "request_handler.h"

namespace transport_catalogue::output {

    RequestHander::RequestHander(backend::TransportCatalogue& tc)
        : transport_catalogue_(&tc) {

    }

    void RequestHander::PrintRequests(std::ostream& os, input::JsonReader& requests) {
        json::Array output;
        detail::OutputQuery temp;
        render::MapRenderer renderer(requests.GetRenderSettings());
        renderer.SetCoordinates(GetCoordinates(), GetStops());
        while(requests.GetQuery(temp)) {
            if(temp.type == "Bus") {
                output.push_back(json::Node(requests.GetBusQuery(*temp.name, temp.id)));
            } else if(temp.type == "Stop") {
                output.push_back(json::Node(requests.GetStopQuery(*temp.name, temp.id)));
            } else if(temp.type == "Map") {
                output.push_back(json::Node(requests.GetMap(temp.id, renderer)));
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
