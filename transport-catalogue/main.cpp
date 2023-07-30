#include <fstream>
#include <iostream>
#include <string_view>

#include "json_reader.h"
#include "request_handler.h"
#include "serialization.h"
#include "map_renderer.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {

        // make base here
//        json::Document input = json::Load(std::cin);

        std::ifstream is("test_19_make_base.json");
        json::Document input = json::Load(is);

        transport_catalogue::backend::TransportCatalogue transport_catalogue;
        transport_catalogue::input::JsonReader requests(transport_catalogue, input);
        std::ofstream ofs(requests.GetSerializationSettings().file, std::ios::binary);

        if(transport_catalogue::serialize::SerializeCatalogue(transport_catalogue, requests.GetRenderSettings(), requests.GetRoutingSettings(), ofs)) {
            return 0;
        } else {
            std::cerr << "Error serializing file"sv;
            return 1;
        }


    } else if (mode == "process_requests"sv) {

        // process requests here
//        json::Document input = json::Load(std::cin);

        std::ifstream is("test_19_process_requests.json");
        json::Document input = json::Load(is);

        transport_catalogue::input::JsonReader requests(input);
        transport_catalogue::render::RenderSettings render_settings;
        transport_catalogue::detail::RoutingSettings routing_settings;
        std::ifstream ifs(requests.GetSerializationSettings().file, std::ios::binary);
        transport_catalogue::backend::TransportCatalogue transport_catalogue = transport_catalogue::serialize::DeserializeCatalogue(ifs, render_settings, routing_settings);

        transport_catalogue::output::RequestHandler request_handler(transport_catalogue, render_settings, routing_settings);
        json::Document result = requests.ProcessRequests(
                [&request_handler](const std::string& name, int id) -> transport_catalogue::detail::BusAnswer{ return request_handler.GetBusQuery(name, id); },
                [&request_handler](const std::string& name, int id) -> transport_catalogue::detail::StopAnswer{ return request_handler.GetStopQuery(name, id); },
                [&request_handler](int id) -> transport_catalogue::detail::MapAnswer{ return request_handler.GetMap(id); },
                [&request_handler](const std::string& from, const std::string& to, int id) -> transport_catalogue::detail::RoutingAnswer{ return request_handler.GetRouteQuery(from, to, id);}
            );
//        json::Print(result, std::cout);
        std::ofstream os("out.json");
        json::Print(result, os);
        return 0;
    } else {
        PrintUsage();
        return 1;
    }
}
