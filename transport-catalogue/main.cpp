#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

int main() {
//    std::istringstream is;
//    is.str(R"({
//           "base_requests": [{
//                   "type": "Bus",
//                   "name": "114",
//                   "stops": ["Морской вокзал", "Ривьерский мост"],
//                   "is_roundtrip": false
//               },
//               {
//                   "type": "Stop",
//                   "name": "Ривьерский мост",
//                   "latitude": 43.587795,
//                   "longitude": 39.716901,
//                   "road_distances": { "Морской вокзал": 850 }
//               },
//               {
//                   "type": "Stop",
//                   "name": "Морской вокзал",
//                   "latitude": 43.581969,
//                   "longitude": 39.719848,
//                   "road_distances": { "Ривьерский мост": 850 }
//               }
//           ],
//           "stat_requests": [
//               { "id": 1, "type": "Stop", "name": "Ривьерский мост" },
//               { "id": 2, "type": "Bus", "name": "114" },
//               { "id": 3, "type": "Stop", "name" : "12345" },
//               { "id": 4, "type": "Bus", "name" : "1456" }
//           ]
//       })");
//    json::Document input = json::Load(is);
    json::Document input = json::Load(std::cin);
    transport_catalogue::backend::TransportCatalogue transport_catalogue;
    transport_catalogue::input::JsonReader requests = {input, transport_catalogue};
    transport_catalogue::output::PrintRequests(std::cout, requests, transport_catalogue);
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
