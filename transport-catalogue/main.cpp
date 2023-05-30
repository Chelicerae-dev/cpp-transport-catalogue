#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

int main() {
    json::Document input = json::Load(std::cin);
    transport_catalogue::backend::TransportCatalogue transport_catalogue;
    transport_catalogue::input::JsonReader requests(input, transport_catalogue);
    transport_catalogue::output::RequestHander request_handler(transport_catalogue);
    request_handler.PrintRequests(std::cout, requests);

    return 0;
}
