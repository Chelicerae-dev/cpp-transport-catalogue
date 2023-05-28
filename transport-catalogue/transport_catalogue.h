#pragma once

#include "geo.h"
#include "domain.h"
#include <deque>
#include <list>
#include <set>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <string_view>
#include <algorithm>

namespace transport_catalogue {
    namespace backend {
    class TransportCatalogue {

        public:
            void AddStop(detail::Stop stop);
            void AddBus(detail::Bus bus);

            void AddStopDistances(detail::Stop* stop, std::map<std::string, int> other_stops);
            int GetStopDistance(detail::Stop* stop, detail::Stop* other_stop);

            detail::Stop* GetStop(std::string_view stop_name);
            detail::Bus* GetBus(std::string_view bus_name);

            detail::Bus MakeBus(std::string& bus_name, std::vector<std::string>& stop_names, bool is_looped);
            detail::Bus MakeBus(detail::BusCreationQuery query);

            detail::BusInfo GetBusInfo(detail::Bus* bus);
            detail::StopInfo GetStopInfo(detail::Stop* stop);

            std::vector<detail::Bus*> GetAllBuses();
            std::vector<detail::Stop*> GetAllStops();

        private:
            std::list<detail::Stop> stops_;
            std::unordered_map<std::string_view, detail::Stop*> stopname_to_stop_;
            std::list<detail::Bus> buses_;
            std::unordered_map<std::string_view, detail::Bus*> busname_to_bus_;
            std::unordered_map<detail::Stop*, std::unordered_map<detail::Stop*, int>> distances_;


    };
    }
}
