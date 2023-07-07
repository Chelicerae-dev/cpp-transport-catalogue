#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

int main() {
    json::Document input = json::Load(std::cin);
    transport_catalogue::backend::TransportCatalogue transport_catalogue;
    transport_catalogue::input::JsonReader requests(transport_catalogue, input);
    transport_catalogue::output::RequestHandler request_handler(transport_catalogue, requests.GetRenderSettings(), requests.GetRoutingSettings());
    //Запрашиваем лямбдами потому что за 2 дня я так и не понял как побороть различные ошибки при попытке передать функции в конкретом объекте
    //Результат всё равно одинаковый
    json::Document result = requests.ProcessRequests(
        [&request_handler](const std::string& name, int id) -> transport_catalogue::detail::BusAnswer{ return request_handler.GetBusQuery(name, id); },
        [&request_handler](const std::string& name, int id) -> transport_catalogue::detail::StopAnswer{ return request_handler.GetStopQuery(name, id); },
        [&request_handler](int id) -> transport_catalogue::detail::MapAnswer{ return request_handler.GetMap(id); },
        [&request_handler](const std::string& from, const std::string& to, int id) -> transport_catalogue::detail::RoutingAnswer{ return request_handler.GetRouteQuery(from, to, id);}
    );
    json::Print(result, std::cout);
    return 0;
}
