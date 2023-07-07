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
        explicit TransportRouter(const detail::RoutingSettings& settings, backend::TransportCatalogue& tranpsort_catalogue);

        detail::Route BuildRoute(graph::VertexId from, graph::VertexId to);

    private:
        detail::RoutingSettings settings_;
        graph::Router<detail::Weight> router_;

        detail::RoutingSettings SetRoutingSettings(const detail::RoutingSettings& settings);
        const graph::DirectedWeightedGraph<detail::Weight> RouterGraphInitializer(backend::TransportCatalogue& transport_catalogue, const detail::RoutingSettings& settings);
    };



}
