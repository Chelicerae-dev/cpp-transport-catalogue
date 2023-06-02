#pragma once

#include <iostream>
#include <string>
#include <functional>

#include "domain.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace output {
        class RequestHandler {
        public:
            RequestHandler(backend::TransportCatalogue& tc, const render::RenderSettings& render_settings);

            //методы для обработки запроса
            detail::MapAnswer GetMap(int id);
            detail::BusAnswer GetBusQuery(const std::string& name, int id);
            detail::StopAnswer GetStopQuery(const std::string& name, int id);

        private:
            //оставил здесь связь и приватные методы, так как они нужны только здесь и не имеют отношения к JSON
            backend::TransportCatalogue* transport_catalogue_;
            render::MapRenderer renderer_;

            std::vector<detail::BusCoordinates> GetCoordinates();
            std::vector<detail::Stop*> GetStops();
        };
    }
}
