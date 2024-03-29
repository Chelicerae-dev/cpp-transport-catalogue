#pragma once

#include <iostream>
#include <string>

//#include "json.h"
#include "json_builder.h"
#include "domain.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace transport_catalogue::input {
    class JsonReader {
    public:
        JsonReader() = default;
        JsonReader(json::Document& input);
        JsonReader(backend::TransportCatalogue& transport_catalogue, json::Document& input);
        bool GetQuery(detail::OutputQuery& output);
        const render::RenderSettings& GetRenderSettings() const;
        const detail::RoutingSettings& GetRoutingSettings() const;
        const detail::SerializationSettings& GetSerializationSettings() const;

        //метод для обработки всех запросов из JSON и ответа в виде JSON
        json::Document ProcessRequests(std::function<detail::BusAnswer(const std::string&, int)> bus_proc,
                                       std::function<detail::StopAnswer(const std::string&, int)> stop_proc,
                                       std::function<detail::MapAnswer(int)> map_proc,
                                       std::function<detail::RoutingAnswer(const std::string&, const std::string&, int)> routing_proc);

    private:
        std::deque<detail::StopDistancesQuery> stops_query_;
        std::deque<detail::BusCreationQuery> buses_query_;
        std::deque<detail::OutputQuery> requests_query_;
        render::RenderSettings render_settings_;
        detail::RoutingSettings routing_settings_;
        detail::SerializationSettings serialization_settings_;
        backend::TransportCatalogue* catalogue_;


        void ParseStop(json::Dict* node);
        void ParseDistances(std::string_view name, json::Dict* line);
        void ParseBus(json::Dict* node);
        void ParseRequest(const json::Dict& node);
        void ParseRenderSettings(const json::Dict& node);
        svg::Color ParseSvgColor(const json::Node& node);
        void ParseSerializationSettings(const json::Dict& node);
    };
}
