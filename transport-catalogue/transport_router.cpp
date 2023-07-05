#include "transport_router.h"

namespace transport_catalogue::routing {
TransportRouter::TransportRouter(const graph::DirectedWeightedGraph<detail::Weight>& graph)
    : graph_(graph), router_(graph_) {
}

std::optional<graph::Router<detail::Weight>::RouteInfo> TransportRouter::BuildRoute(graph::VertexId from, graph::VertexId to) const {
    return router_.BuildRoute(from, to);
}

const graph::Edge<detail::Weight>& TransportRouter::GetEdge(graph::EdgeId edge_id) const {
    return graph_.GetEdge(edge_id);
}

TransportRouter RouterInitializer(backend::TransportCatalogue& transport_catalogue) {
    std::vector<graph::Edge<detail::Weight>> graph_data = transport_catalogue.GetGraphData();
    graph::DirectedWeightedGraph<detail::Weight> route_graph(transport_catalogue.GetVertexCount());
    for(const graph::Edge<detail::Weight>& edge : graph_data) {
        route_graph.AddEdge(edge);
    }
    return TransportRouter(route_graph);
}
} //namespace routing
