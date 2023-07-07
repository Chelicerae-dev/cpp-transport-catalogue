#include "transport_router.h"

namespace transport_catalogue::routing {
    TransportRouter::TransportRouter(const detail::RoutingSettings& settings, backend::TransportCatalogue& transport_catalogue)
        : settings_(SetRoutingSettings(settings))
        , router_(RouterGraphInitializer(transport_catalogue, settings_)) {
    }

    detail::Route TransportRouter::BuildRoute(graph::VertexId from, graph::VertexId to) {
        std::optional<graph::Router<detail::Weight>::RouteInfo> raw_route = router_.BuildRoute(from, to);
        detail::Route result;
        if(raw_route.has_value()) {
            result.exists = true;
            result.time = raw_route.value().weight.value;
            result.items = std::vector<detail::RouteItem>(raw_route.value().edges.size());
            for(int i = 0; i < result.items->size(); ++i) {
                graph::Edge<detail::Weight> edge = router_.GetEdge(raw_route.value().edges.at(i));
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
    }

//    const graph::Edge<detail::Weight> TransportRouter::GetEdge(graph::EdgeId edge_id) const {
//        return graph_.GetEdge(edge_id);
//    }

    //я не вижу более простого способоа - учитывая "внешний" характер router и проблемы, с которыми я сталкиваюсь при попытке прикрутить к нему
    //конструктор по умолчанию, копирующий, перемещающий - ведь я не могу не инициализировать router_ перед телом конструктора
    //Поэтомуя немного переработал этот метод для использования в инициализаторе полей класса в конструкторе
    const graph::DirectedWeightedGraph<detail::Weight> TransportRouter::RouterGraphInitializer(backend::TransportCatalogue& transport_catalogue, const detail::RoutingSettings& settings) {
        std::vector<graph::Edge<detail::Weight>> graph_data = transport_catalogue.GetGraphData(settings);
        graph::DirectedWeightedGraph<detail::Weight> route_graph(transport_catalogue.GetVertexCount());
        for(const graph::Edge<detail::Weight>& edge : graph_data) {
            route_graph.AddEdge(edge);
        }
        return route_graph;
    }

    detail::RoutingSettings TransportRouter::SetRoutingSettings(const detail::RoutingSettings& settings) {
        //скорость в кмч поступает, нам же нужны метры в минуту
        return {settings.bus_wait_time, settings.bus_velocity * 1000 / 60};
    }
} //namespace routing

