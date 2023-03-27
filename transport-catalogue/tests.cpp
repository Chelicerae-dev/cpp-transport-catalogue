#include "input_reader.h"
//#include "stat_reader.h"
#include "transport_catalogue.h"
#include <iostream>
#include <iomanip>

namespace transport_catalogue::tests {
void TestAddingToCatalogue() {

}

void TestInput() {
//    using namespace std::string_literals;
//    //создаём поток ввода
//    std::istringstream input;
//    input.str("10\nStop Tolstopaltsevo: 55.611087, 37.208290\nStop Marushkino: 55.595884, 37.209755\nBus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\nBus 750: Tolstopaltsevo - Marushkino - Rasskazovka\nStop Rasskazovka: 55.632761, 37.333324\nStop Biryulyovo Zapadnoye: 55.574371, 37.651700\nStop Biryusinka: 55.581065, 37.648390\nStop Universam: 55.587655, 37.645687\nStop Biryulyovo Tovarnaya: 55.592028, 37.653656\nStop Biryulyovo Passazhirskaya: 55.580999, 37.659164"s);
//    //проверяем
//    {
//        other.AddStop({"Tolstopaltsevo"s, {55.611087, 37.208290}});
//        other.AddStop({"Marushkino"s, {55.595884, 37.209755}});
//        other.AddStop({"Rasskazovka"s,{55.632761, 37.333324}});
//        other.AddStop({"Biryulyovo Zapadnoye"s,{55.574371, 37.651700}});
//        other.AddStop({"Biryusinka"s,{55.581065, 37.648390}});
//        other.AddStop({"Universam"s,{55.587655, 37.645687}});
//        other.AddStop({"Biryulyovo Tovarnaya"s,{55.592028, 37.653656}});
//        other.AddStop({"Biryulyovo Passazhirskaya"s,{55.580999, 37.659164}});
//    }
}

void RunTests() {
    TestAddingToCatalogue();
    TestInput();

}

}
