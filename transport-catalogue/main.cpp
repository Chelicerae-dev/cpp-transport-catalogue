#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

int main() {
    json::Document input = json::Load(std::cin);
    transport_catalogue::backend::TransportCatalogue transport_catalogue;
    transport_catalogue::input::JsonReader requests(input, transport_catalogue);
    transport_catalogue::output::RequestHander request_handler(transport_catalogue);
    transport_catalogue::render::MapRenderer map_renderer(requests.GetRenderSettings());
    map_renderer.SetCoordinates(request_handler.GetCoordinates(), request_handler.GetStops());
    request_handler.PrintRequests(std::cout, requests, map_renderer);
//    map_renderer.Print(std::cout);
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
     * с ответами.
     * Вывести в stdout ответы в виде JSON
     */

    return 0;
}
