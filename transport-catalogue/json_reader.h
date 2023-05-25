#pragma once

#include <iostream>
#include <string>

#include "json.h"
#include "transport_catalogue.h"

namespace transport_catalogue::input {
    class JsonReader {
    public:
        JsonReader() = default;
        JsonReader(json::Document input, backend::TransportCatalogue& transport_catalogue);
        bool GetQuery(detail::OutputQuery& output);


    private:
        std::deque<detail::StopDistancesQuery> stops_query_;
        std::deque<detail::BusCreationQuery> buses_query_;
        std::deque<detail::OutputQuery> requests_query_;


        void ParseStop(backend::TransportCatalogue& transport_catalogue, json::Dict* node);
        void ParseDistances(std::string_view name, json::Dict* line);
        void ParseBus(json::Dict* node);
        void ParseRequest(const json::Dict& node);
    };


}
