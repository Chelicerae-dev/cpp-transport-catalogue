#include "request_handler.h"

namespace transport_catalogue::output {

    RequestHandler::RequestHandler(backend::TransportCatalogue& tc, const render::RenderSettings& render_settings)
        : transport_catalogue_(&tc), renderer_(render_settings) {
        renderer_.SetCoordinates(GetCoordinates(), GetStops());
    }

    detail::MapAnswer RequestHandler::GetMap(int id) {
        return {id, renderer_.Print()};
    }

    detail::BusAnswer RequestHandler::GetBusQuery(const std::string& name, int id) {
        detail::BusAnswer result;
        result.id = id;
        detail::Bus* bus = transport_catalogue_->GetBus(name);
        if(bus != nullptr) {
            result.exists = true;
            result.bus_info = transport_catalogue_->GetBusInfo(bus);
        }
        return result;
    }

    detail::StopAnswer RequestHandler::GetStopQuery(const std::string& name, int id) {
        detail::StopAnswer result;
        result.id = id;
        detail::Stop* stop = transport_catalogue_->GetStop(name);
        if(stop != nullptr) {
            result.exists = true;
            result.stop_info = transport_catalogue_->GetStopInfo(stop);
        }
        return result;
    }

    std::vector<detail::BusCoordinates> RequestHandler::GetCoordinates() {
        std::vector<detail::BusCoordinates> result;
        std::vector<detail::Bus*> buses = transport_catalogue_->GetAllBuses();
        for(detail::Bus* bus : buses) {
            std::vector<geo::Coordinates> coords;
            for(detail::Stop* stop : bus->stops) {
                coords.push_back(stop->location);
            }
            if(!bus->is_looped) {
                std::for_each(bus->stops.rbegin() + 1, bus->stops.rend(), [&coords](detail::Stop* stop){
                    coords.push_back(stop->location);
                });
            }
            result.push_back({bus, coords});
        }
        return result;
    }

    std::vector<detail::Stop*> RequestHandler::GetStops() {
        return transport_catalogue_->GetAllStops();
    }

}
