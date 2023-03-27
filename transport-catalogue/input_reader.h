#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <deque>
#include <map>
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace detail {
        struct StopDistancesQuery {
            std::string name;
            std::map<std::string, int> distances;
        };
    }
    namespace input {
        class InputReader {
        public:
            InputReader(std::istream& is, backend::TransportCatalogue& transport_catalogue);

        private:
            void ParseStop(backend::TransportCatalogue& transport_catalogue, std::string_view temp_string, int64_t pos, int64_t delimiter);
            void ParseDistances(std::string_view name, std::string_view line);

            void ParseBus(std::string_view temp_string, int64_t pos, int64_t delimiter);

            std::deque<detail::StopDistancesQuery> stops_query_;
            std::deque<detail::BusCreationQuery> buses_query_;
        };
    }
}
