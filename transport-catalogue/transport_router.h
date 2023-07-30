#pragma once

#include "domain.h"
#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"

#include <string_view>
#include <optional>
#include <vector>
#include <memory>

namespace transport_catalogue::routing {


    class TransportRouter {
    public:
        explicit TransportRouter(const detail::RoutingSettings& settings, backend::TransportCatalogue& tranpsort_catalogue);
        explicit TransportRouter(const detail::RoutingSettings& settings, detail::RouterSerialization&& router_data);

        detail::Route BuildRoute(graph::VertexId from, graph::VertexId to);

        std::pair<detail::RoutingSettings, graph::Router<detail::Weight>*> GetData();
    private:
        detail::RoutingSettings settings_;
        std::unique_ptr<graph::Router<detail::Weight>> router_ = nullptr;

        detail::RoutingSettings SetRoutingSettings(const detail::RoutingSettings& settings);
        double ConvertSpeed(double bus_velocity) const;
    };



}
