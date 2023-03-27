#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
#include <bits/stdc++.h>

int main() {
    std::istringstream cin;
    cin.str("13\nStop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\nStop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\nBus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\nBus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\nStop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino\nStop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\nStop Biryusinka: 55.581065, 37.64839, 750m to Universam\nStop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\nStop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\nStop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\nBus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\nStop Rossoshanskaya ulitsa: 55.595579, 37.605757\nStop Prazhskaya: 55.611678, 37.603831\n6\nBus 256\nBus 750\nBus 751\nStop Samara\nStop Prazhskaya\nStop Biryulyovo Zapadnoye");
    transport_catalogue::backend::TransportCatalogue transport_catalogue;
    transport_catalogue::input::InputReader input_reader =  transport_catalogue::input::InputReader(cin, transport_catalogue);
    transport_catalogue::output::StatReader stat_reader = transport_catalogue::output::StatReader(cin, transport_catalogue);
    stat_reader.PrintQueries();
}

