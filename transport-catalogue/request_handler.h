#pragma once

#include <iostream>
#include <string>

#include "map_renderer.h"
#include "json_reader.h"
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace output {
        class RequestHander {
        public:
            RequestHander(backend::TransportCatalogue& tc);

            void PrintRequests(std::ostream& os, input::JsonReader& requests);

        private:
            //оставил здесь связь и приватные методы, так как они нужны только здесь и не имеют отношения к JSON
            backend::TransportCatalogue* transport_catalogue_;

            std::vector<detail::BusCoordinates> GetCoordinates();
            std::vector<detail::Stop*> GetStops();
        };
    }
}
