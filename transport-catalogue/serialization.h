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

    //Сериализуем TransportCatalogue и возвращаем флаг статуса записи в файл
    bool SerializeCatalogue(backend::TransportCatalogue& catalogue, const render::RenderSettings& render_settings, routing::TransportRouter& t_router, std::ofstream& out);
    //десериализуем из потока в TransportCatalogue и заполняем выходной аргумент rs_out
    backend::TransportCatalogue DeserializeCatalogue(std::ifstream& in,
                                                     render::RenderSettings& rs_out,
                                                     detail::RouterSerialization& router_data,
                                                     detail::RoutingSettings& routing_settings);

    //Сначала добавляем вершины, заодно используем их ID
    void AddVertices(const std::unordered_map<detail::Stop*, detail::StopVertices>& vertices_list, proto_catalogue::TransportCatalogue& pc, std::map<detail::Stop*, uint32_t>& stop_ids);

    //делаем прото-остановки и добавляем их в каталог на сериалиазацию
    //ощущаю себя создателем миров, создающим протозвёзды, протобактерии... и прото-остановки с прото-автобусами :)
    void AddProtoStops(const std::vector<detail::Stop*>& stops,
                        proto_catalogue::TransportCatalogue& pc,
                        std::map<detail::Stop*, uint32_t>& stop_ids);

    //Добавляем прото-дистанции. Для этого нам пригодятся ID остановок
    void AddProtoDistances(const std::unordered_map<detail::Stop*, std::unordered_map<detail::Stop*, int>>& distances,
                            proto_catalogue::TransportCatalogue& pc,
                            const std::map<detail::Stop*, uint32_t>& stop_ids);
    //добавляем прото-маршруты
    void AddProtoBuses(const std::vector<detail::Bus*>& buses,
                        proto_catalogue::TransportCatalogue& pc,
                        std::map<detail::Stop*, uint32_t>& stop_ids);

    //Сериализуем настройки рендера
    void AddRenderSettings(const render::RenderSettings& render_settings,
                           proto_catalogue::TransportCatalogue& pc);

    proto_map::RenderSettings MakeRenderSettings(const render::RenderSettings& render_settings);
    proto_svg::Color MakeColor(const svg::Color& color);

    //Парсим остановки из proto_catalogue и добавляем их в backend::TransportCatalogue и stop_ids
    void ParseProtoStops(proto_catalogue::TransportCatalogue& pc,
                         backend::TransportCatalogue& tc, std::map<uint32_t,
                         detail::Stop*>& stop_ids);
    //Парсим и добавляем дистанции в backend::TransportCatalogue
    void ParseProtoDistances(proto_catalogue::TransportCatalogue& pc,
                             backend::TransportCatalogue& tc, std::map<uint32_t,
                             detail::Stop*>& stop_ids);
    //Парсим и добавляем маршруты в backend::TransportCatalogue
    void ParseProtoBuses(proto_catalogue::TransportCatalogue& pc,
                         backend::TransportCatalogue& tc, std::map<uint32_t,
                         detail::Stop*>& stop_ids);

    render::RenderSettings ParseRenderSettings(proto_catalogue::TransportCatalogue& pc);
    svg::Color RenderColor(const proto_svg::Color& color);

    std::unordered_map<std::string_view, detail::StopVertices> ParseVertices(proto_catalogue::TransportCatalogue& pc, std::map<uint32_t, detail::Stop*>& stop_ids) ;


    //Рабоатаем с роутером
    void AddRouter(routing::TransportRouter& t_router, proto_catalogue::TransportCatalogue& pc,
                                            const std::map<std::string_view, uint32_t>& stop_ids,
                                            const std::map<std::string_view, uint32_t>& bus_ids);
//    graph::DirectedWeightedGraph<detail::Weight>


    proto_graph::Weight MakeProtoWeightBus(const detail::Weight& input, const std::map<std::string_view, uint32_t>& bus_ids);
    proto_graph::Weight MakeProtoWeightStop(const detail::Weight& input, const std::map<std::string_view, uint32_t>& stop_ids);

    //Парсим элементы графа для сборки transport_router
    detail::Weight MakeWeight(const proto_graph::Weight& pw,
                              const std::map<uint32_t, detail::Stop*>& stop_ids,
                              const std::vector<detail::Bus*>& buses);
    std::vector<graph::Edge<detail::Weight>> ParseGraphEdges(proto_catalogue::TransportCatalogue& pc, backend::TransportCatalogue& tc,
                                                             const std::map<uint32_t, detail::Stop*>& stop_ids,
                                                             const std::vector<detail::Bus*>& buses);
    std::vector<std::vector<size_t>> ParseIncidenceList(proto_catalogue::TransportCatalogue& pc);

    std::vector<std::vector<std::optional<graph::Router<detail::Weight>::RouteInternalData>>> ParseRoutesInternalData(proto_catalogue::TransportCatalogue& pc,
                                                                                                                      const std::map<uint32_t, detail::Stop*>& stop_ids,
                                                                                                                      const std::vector<detail::Bus*>& buses);

    detail::RoutingSettings ParseRoutingSettings(proto_catalogue::TransportCatalogue& pc);

} //namespace transport_catalogue::serialize
