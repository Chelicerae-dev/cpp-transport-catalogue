#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main() {
    transport_catalogue::backend::TransportCatalogue transport_catalogue;
    transport_catalogue::input::InputReader input_reader =  transport_catalogue::input::InputReader(std::cin, transport_catalogue);
    transport_catalogue::output::StatReader stat_reader = transport_catalogue::output::StatReader(transport_catalogue);
    stat_reader.PrintQueries();
}

