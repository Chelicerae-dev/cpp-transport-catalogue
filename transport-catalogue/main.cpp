#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

int main() {
    std::stringstream strm;
    std::string str = R"({
    "base_requests": [
        {
            "is_roundtrip": true,
            "name": "HAya3duOCPyh",
            "stops": [
                "e4VumNOSqA8V",
                "mUPaFAe3qdv",
                "e4VumNOSqA8V"
            ],
            "type": "Bus"
        },
        {
            "is_roundtrip": false,
            "name": "Pp4fU5q",
            "stops": [
                "e4VumNOSqA8V",
                "mUPaFAe3qdv"
            ],
            "type": "Bus"
        },
        {
            "is_roundtrip": false,
            "name": "t0k 9 ywgz15LVQQLWEw",
            "stops": [
                "mUPaFAe3qdv",
                "e4VumNOSqA8V"
            ],
            "type": "Bus"
        },
        {
            "latitude": 45.6231,
            "longitude": 37.6244,
            "name": "e4VumNOSqA8V",
            "road_distances": {
                "mUPaFAe3qdv": 912575
            },
            "type": "Stop"
        },
        {
            "latitude": 40.2168,
            "longitude": 39.3595,
            "name": "mUPaFAe3qdv",
            "road_distances": {
                "e4VumNOSqA8V": 843069
            },
            "type": "Stop"
        }
    ],
    "render_settings": {
        "bus_label_font_size": 19054,
        "bus_label_offset": [
            -50880,
            11210.1
        ],
        "color_palette": [
            "orange",
            "blue",
            [
                150,
                242,
                56
            ],
            [
                31,
                40,
                8
            ],
            "khaki",
            "bisque",
            [
                94,
                250,
                155,
                0.403971
            ],
            "purple",
            [
                253,
                255,
                79
            ],
            [
                214,
                53,
                228,
                0.546366
            ],
            [
                229,
                149,
                20
            ],
            [
                251,
                119,
                117
            ],
            [
                48,
                138,
                94
            ],
            [
                14,
                177,
                230
            ],
            "magenta",
            [
                32,
                241,
                32
            ],
            [
                89,
                63,
                237
            ],
            "gray",
            [
                143,
                204,
                8,
                0.260354
            ],
            [
                77,
                251,
                241
            ],
            [
                22,
                201,
                241,
                0.290294
            ],
            "orchid",
            [
                0,
                67,
                130
            ],
            [
                161,
                1,
                29
            ],
            "lavender",
            [
                217,
                39,
                158
            ],
            "indigo",
            "thistle",
            [
                145,
                91,
                201
            ],
            [
                44,
                143,
                176
            ],
            "lime",
            [
                174,
                149,
                29,
                0.314756
            ],
            "khaki",
            [
                11,
                138,
                115,
                0.321871
            ],
            [
                143,
                94,
                106,
                0.620115
            ],
            [
                111,
                180,
                40
            ],
            "wheat",
            "teal",
            "white",
            [
                201,
                193,
                77
            ],
            "cyan",
            "bisque",
            [
                201,
                108,
                38
            ],
            [
                85,
                98,
                227
            ],
            [
                170,
                41,
                170
            ],
            "orange",
            [
                208,
                54,
                82
            ],
            "blue",
            "white",
            [
                3,
                34,
                192
            ],
            [
                31,
                39,
                169,
                0.76035
            ],
            [
                209,
                250,
                12
            ],
            "aqua",
            [
                238,
                233,
                134,
                0.18853
            ],
            "lavender",
            "wheat",
            [
                214,
                113,
                144,
                0.919725
            ],
            [
                31,
                211,
                158
            ],
            "fuchsia",
            [
                146,
                114,
                144
            ],
            "khaki",
            "wheat",
            "orchid",
            "cornsilk",
            [
                75,
                52,
                55,
                0.639386
            ],
            "orange",
            [
                40,
                122,
                106,
                0.523562
            ],
            [
                8,
                101,
                166,
                0.722518
            ],
            "magenta",
            [
                24,
                88,
                69,
                0.844297
            ],
            "white",
            [
                249,
                85,
                172
            ],
            [
                156,
                93,
                3
            ],
            [
                4,
                36,
                217,
                0.0911362
            ],
            "purple",
            [
                108,
                93,
                99
            ],
            "lavender",
            "tan",
            [
                82,
                133,
                108
            ],
            "bisque",
            "coral",
            [
                63,
                161,
                172,
                0.932561
            ],
            [
                110,
                235,
                251
            ],
            [
                94,
                21,
                199
            ],
            "chocolate",
            [
                113,
                82,
                186,
                0.836677
            ],
            [
                210,
                140,
                119
            ],
            [
                114,
                48,
                97,
                0.525546
            ],
            "gold",
            "sienna",
            [
                87,
                175,
                233
            ],
            "lime",
            [
                66,
                237,
                140
            ],
            [
                68,
                12,
                232
            ]
        ],
        "height": 5628.85,
        "line_width": 85599.2,
        "padding": 222.308,
        "stop_label_font_size": 63654,
        "stop_label_offset": [
            73780.4,
            -79365.2
        ],
        "stop_radius": 7531.33,
        "underlayer_color": "peru",
        "underlayer_width": 41046.3,
        "width": 61732.6
    },
    "stat_requests": [
        {
            "id": 1380626589,
            "name": "t0k 9 ywgz15LVQQLWEw",
            "type": "Bus"
        },
        {
            "id": 325152343,
            "name": "mUPaFAe3qdv",
            "type": "Stop"
        },
        {
            "id": 325152343,
            "name": "mUPaFAe3qdv",
            "type": "Stop"
        },
        {
            "id": 1805321361,
            "name": "SnnjuqtQiHAnWzGxZ1q7VH",
            "type": "Bus"
        },
        {
            "id": 325152343,
            "name": "mUPaFAe3qdv",
            "type": "Stop"
        },
        {
            "id": 1380626589,
            "name": "t0k 9 ywgz15LVQQLWEw",
            "type": "Bus"
        },
        {
            "id": 1380626589,
            "name": "t0k 9 ywgz15LVQQLWEw",
            "type": "Bus"
        },
        {
            "id": 325152343,
            "name": "mUPaFAe3qdv",
            "type": "Stop"
        },
        {
            "id": 360902539,
            "name": "Pp4fU5q",
            "type": "Bus"
        },
        {
            "id": 166072951,
            "type": "Map"
        }
    ]
})";
    strm.str(str);
    json::Document input = json::Load(strm);
//    json::Document input = json::Load(std::cin);
    transport_catalogue::backend::TransportCatalogue transport_catalogue;
    transport_catalogue::input::JsonReader requests(transport_catalogue, input);
    transport_catalogue::output::RequestHandler request_handler(transport_catalogue, requests.GetRenderSettings());
    //Запрашиваем лямбдами потому что за 2 дня я так и не понял как побороть различные ошибки при попытке передать функции в конкретом объекте
    //Результат всё равно одинаковый
    json::Document result = requests.ProcessRequests(
        [&request_handler](const std::string& name, int id) -> transport_catalogue::detail::BusAnswer{ return request_handler.GetBusQuery(name, id); },
        [&request_handler](const std::string& name, int id) -> transport_catalogue::detail::StopAnswer{ return request_handler.GetStopQuery(name, id); },
        [&request_handler](int id) -> transport_catalogue::detail::MapAnswer{ return request_handler.GetMap(id); }
    );
    json::Print(result, std::cout);
    return 0;
}
