#pragma once

#include "domain.h"
#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"

#include <string_view>
#include <optional>
#include <vector>

namespace transport_catalogue::routing {


    class TransportRouter {
    public:
        explicit TransportRouter(const graph::DirectedWeightedGraph<detail::Weight>& graph);

        std::optional<graph::Router<detail::Weight>::RouteInfo> BuildRoute(graph::VertexId from, graph::VertexId to) const;
        const graph::Edge<detail::Weight>& GetEdge(graph::EdgeId edge_id) const;
    private:
        graph::DirectedWeightedGraph<detail::Weight> graph_;
        graph::Router<detail::Weight> router_;
    };

    TransportRouter RouterInitializer(backend::TransportCatalogue& transport_catalogue);

}
