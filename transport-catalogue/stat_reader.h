#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace detail {
        enum QueryType {
            BusQuery, StopQuery
        };

        struct OutputQuery {
            std::string name;
            size_t place;
            QueryType type;
            bool exists;
        };
    }

    namespace output {
        class StatReader {

          public:
            StatReader(backend::TransportCatalogue& transport_catalogue);
            void PrintQueries();
          private:
            void ParseBusQuery(backend::TransportCatalogue& transport_catalogue, std::string_view temp_string, int64_t pos);
            void ParseStopQuery(backend::TransportCatalogue& transport_catalogue, std::string_view temp_string, int64_t pos);

            std::vector<detail::OutputQuery> queries_;
            std::vector<detail::BusInfo> bus_queries_;
            std::vector<detail::StopInfo> stop_queries_;
        };
    }
}
