#pragma once

#include "geo.h"
#include <string>
#include <set>
#include <vector>
#include <map>
#include <optional>


namespace transport_catalogue {
    namespace detail {
        struct Bus;

        struct Stop {
            std::string name;
            geo::Coordinates location;
            std::set<Bus*> buses{};
            bool operator<(Stop another) {
                return (this->name < another.name);
            }
        };

        struct Bus {
            std::string name;
            std::vector<Stop*> stops;
            bool is_looped;
            bool operator<(const Bus& other);
        };

        struct BusCreationQuery {
            std::string name;
            std::vector<std::string> stops;
            bool is_looped;

        };

        struct BusInfo {
            std::string_view name;
            int stop_count;
            int unique_stop_count;
            int length;
            double curvature;
        };

        struct StopInfo {
            std::string_view name;
            std::set<std::string> buses;
        };

        struct StopDistancesQuery {
            std::string name;
            std::map<std::string, int> distances;
        };

        enum QueryType {
            BusQuery, StopQuery
        };

        struct OutputQuery {
            OutputQuery() = default;
            OutputQuery(int64_t id, std::string type);
            OutputQuery(int64_t id, std::string type, std::string name);
            int64_t id;
            std::string type;
            std::optional<std::string> name;
        };

        struct BusCoordinates {
            Bus* bus;
            std::vector<geo::Coordinates> coords;
        };


    }

}
