#pragma once

#include <transport_catalogue.pb.h>
#include <graph.pb.h>
#include <transport_router.pb.h>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "transport_router.h"

#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <variant>
#include <exception>

namespace transport_catalogue::serialize {
//Интересное решение стилистическое (и техническое, я так понял анонимный неймспейс создаёт, по сути, аналог статичных методов, только без класса)
//Тогда и использование этих функций в хедере ("для внешнего пользователя") действительно не только не нужно, но и не имеет смысла

    //Сериализуем TransportCatalogue и возвращаем флаг статуса записи в файл
    bool SerializeCatalogue(backend::TransportCatalogue& catalogue, const render::RenderSettings& render_settings, routing::TransportRouter& t_router, std::ofstream& out);
    //десериализуем из потока в TransportCatalogue и заполняем выходной аргумент rs_out
    backend::TransportCatalogue DeserializeCatalogue(std::ifstream& in,
                                                     render::RenderSettings& rs_out,
                                                     detail::RouterSerialization& router_data,
                                                     detail::RoutingSettings& routing_settings);

} //namespace transport_catalogue::serialize
