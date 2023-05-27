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
            //std::map<std::string_view, int> distance_to_stop{};
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


    }

}

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
