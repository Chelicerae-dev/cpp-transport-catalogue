#include "transport_router.h"

namespace transport_catalogue::routing {
    TransportRouter::TransportRouter(const detail::RoutingSettings& settings, backend::TransportCatalogue& transport_catalogue)
        : settings_(SetRoutingSettings(settings))
    {
        std::vector<graph::Edge<detail::Weight>> graph_data = transport_catalogue.GetGraphData(settings_);
        graph::DirectedWeightedGraph<detail::Weight> route_graph(transport_catalogue.GetVertexCount());
        for(const graph::Edge<detail::Weight>& edge : graph_data) {
            route_graph.AddEdge(edge);
        }
        router_ = std::make_unique<graph::Router<detail::Weight>>(route_graph);
    }

    detail::Route TransportRouter::BuildRoute(graph::VertexId from, graph::VertexId to) {
        if(router_) {
            std::optional<graph::Router<detail::Weight>::RouteInfo> raw_route = router_->BuildRoute(from, to);
            detail::Route result;
            if(raw_route.has_value()) {
                result.exists = true;
                result.time = raw_route.value().weight.value;
                result.items = std::vector<detail::RouteItem>(raw_route.value().edges.size());
                for(int i = 0; i < result.items->size(); ++i) {
                    graph::Edge<detail::Weight> edge = router_->GetEdge(raw_route.value().edges.at(i));
                    detail::RouteItem temp;
                    if(edge.weight.is_wait) {
                        temp.type = detail::RouteItem::RouteItemType::WAIT;
                        temp.time = settings_.bus_wait_time;
                    } else {
                        temp.type = detail::RouteItem::RouteItemType::BUS;
                        temp.span_count = edge.weight.span;
                        temp.time = edge.weight.value;
                    }

                    temp.name = std::string(edge.weight.name);
                    result.items.value()[i] = temp;
                 }
            }
            return result;
        } else {
            throw std::logic_error("Router is not initialized");
        }

    }

    detail::RoutingSettings TransportRouter::SetRoutingSettings(const detail::RoutingSettings& settings) {
        //скорость в кмч поступает, нам же нужны метры в минуту
        return {settings.bus_wait_time, ConvertSpeed(settings.bus_velocity)};
    }

    double TransportRouter::ConvertSpeed(double bus_velocity) const {
        return bus_velocity * 1000 / 60;
    }
} //namespace routing

