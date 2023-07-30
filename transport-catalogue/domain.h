#pragma once

#include "geo.h"
#include "graph.h"
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
            OutputQuery(int64_t _id, std::string _type, std::string _from, std::string _to)
                :id(_id), type(_type), from(_from), to(_to) {}

            int64_t id;
            std::string type;
            std::optional<std::string> name;
            std::optional<std::string> from;
            std::optional<std::string> to;
        };

        struct BusCoordinates {
            Bus* bus;
            std::vector<geo::Coordinates> coords;
        };

        struct BusAnswer {
            int id;
            std::optional<BusInfo> bus_info;
            bool exists = false;
            std::string error_message = "not found";
        };

        struct StopAnswer {
            int id;
            std::optional<StopInfo> stop_info;
            bool exists = false;
            std::string error_message = "not found";
        };

        struct MapAnswer {
            int id;
            std::string map;
        };

        struct RouteItem {
            enum RouteItemType {
                WAIT, BUS
            };

            RouteItemType type;
            std::string name;
            std::optional<int> span_count;
            std::optional<double> time;
        };

        struct RoutingAnswer {
            int id;
            std::optional<double> total_time;
            std::optional<std::vector<RouteItem>> items;
            bool exists = false;
        };

        struct RoutingSettings {
            uint16_t bus_wait_time;
            double bus_velocity;
        };

        struct Weight {
            double value;
            bool is_wait = false;
            int span;
            std::string_view name;
            Weight operator+(const Weight& other) const;
            bool operator<(const Weight& other) const;
            bool operator>(const Weight& other) const;
        };

        struct StopVertices {
            Stop* stop;
            //bool is_looped;
            graph::VertexId wait;
            graph::VertexId bus;
        };

        struct RouteGraphData {
            std::string_view bus_name;
            std::vector<StopVertices> stops_to_vertex;
            std::vector<graph::Edge<Weight>> edges;
        };

        struct Route {
            bool exists = false;
            double time = 0.;
            std::optional<std::vector<RouteItem>> items;
        };

        struct SerializationSettings {
            std::string file;
        };
    }
}
