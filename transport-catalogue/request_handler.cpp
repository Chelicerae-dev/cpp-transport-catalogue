#include "request_handler.h"

namespace transport_catalogue::output {

    RequestHandler::RequestHandler(backend::TransportCatalogue& tc, const render::RenderSettings& render_settings)
        : transport_catalogue_(&tc), renderer_(render_settings), router_(routing::RouterInitializer(tc)) {
        renderer_.SetCoordinates(GetCoordinates(), GetStops());
    }

    detail::MapAnswer RequestHandler::GetMap(int id) {
        return {id, renderer_.Print()};
    }

    detail::RoutingAnswer RequestHandler::GetRouteQuery(const std::string& from, const std::string& to, int id) {
        detail::RoutingAnswer result;
        result.id = id;
        auto ver_from = transport_catalogue_->GetStopVertex(from);
        auto ver_to = transport_catalogue_->GetStopVertex(to);
        std::optional<graph::Router<detail::Weight>::RouteInfo> route = router_.BuildRoute(ver_from, ver_to);
        if(route.has_value()) {
            result.exists = true;
            result.total_time = route.value().weight.value;
            result.items = std::vector<detail::RoutingAnswerItem>();
            for(graph::EdgeId edge_id : route.value().edges) {
                graph::Edge<detail::Weight> edge = router_.GetEdge(edge_id);
                detail::RoutingAnswerItem item;
                if(edge.weight.is_wait) {
                    item.type = "Wait";
                } else {
                    item.type = "Bus";
                    item.span_count = edge.weight.span;
                }
                item.name = edge.weight.name;
                item.time = edge.weight.value;
                result.items.value().push_back(item);
            }
        }
        return result;
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
