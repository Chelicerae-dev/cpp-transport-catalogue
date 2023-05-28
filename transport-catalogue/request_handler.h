#pragma once

#include <iostream>
#include <string>


#include "json.h"
#include "json_reader.h"
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace output {
        class RequestHander {
        public:
            RequestHander(backend::TransportCatalogue& tc);

            void PrintRequests(std::ostream& os, input::JsonReader& requests, render::MapRenderer& renderer);
            std::vector<detail::BusCoordinates> GetCoordinates();
            std::vector<detail::Stop*> GetStops();

        private:
            backend::TransportCatalogue* transport_catalogue_;

            json::Dict GetBusQuery(const std::string& name, int id);
            json::Dict GetStopQuery(const std::string& name, int id);
            json::Dict GetMap(int id, render::MapRenderer& renderer);
        };
    }
}



/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
/*
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
*/