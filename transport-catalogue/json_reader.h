#pragma once

#include <iostream>
#include <string>

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace transport_catalogue::input {
    class JsonReader {
    public:
        JsonReader() = default;
        JsonReader(json::Document input, backend::TransportCatalogue& transport_catalogue);
        bool GetQuery(detail::OutputQuery& output);
        const render::RenderSettings& GetRenderSettings() const;

        json::Dict GetBusQuery(const std::string& name, int id);
        json::Dict GetStopQuery(const std::string& name, int id);
        json::Dict GetMap(int id, render::MapRenderer& renderer);

    private:
        std::deque<detail::StopDistancesQuery> stops_query_;
        std::deque<detail::BusCreationQuery> buses_query_;
        std::deque<detail::OutputQuery> requests_query_;
        render::RenderSettings render_settings_;
        backend::TransportCatalogue* catalogue_;


        void ParseStop(json::Dict* node);
        void ParseDistances(std::string_view name, json::Dict* line);
        void ParseBus(json::Dict* node);
        void ParseRequest(const json::Dict& node);
        void ParseRenderSettings(const json::Dict& node);
        svg::Color ParseSvgColor(const json::Node& node);
    };
}
