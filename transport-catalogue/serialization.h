#pragma once

#include <transport_catalogue.pb.h>
#include "transport_catalogue.h"
#include "json_reader.h"

#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <exception>

namespace transport_catalogue::serialize {

    //Сериализуем TransportCatalogue и возвращаем флаг статуса записи в файл
    bool SerializeCatalogue(backend::TransportCatalogue& catalogue,
                            const render::RenderSettings& render_settings,
                            const detail::RoutingSettings& routing_settings,
                            std::ofstream& out);
    //десериализуем из потока в TransportCatalogue и заполняем выходной аргумент rs_out
    backend::TransportCatalogue DeserializeCatalogue(std::ifstream& in, render::RenderSettings& rs_out, detail::RoutingSettings& routing_settings);

    //делаем прото-остановки и добавляем их в каталог на сериалиазацию
    //ощущаю себя создателем миров, создающим протозвёзды, протобактерии... и прото-остановки с прото-автобусами :)
    void AddProtoStops(const std::vector<detail::Stop*>& stops,
                        proto_catalogue::TransportCatalogue& pc,
                        std::map<detail::Stop*, uint32_t>& stop_ids);
    //Добавляем прото-дистанции. Для этого нам пригодятся ID остановок
    //Везде используем .at() без проверок, ведь что проверять? ID собираются из готового справочника на момент, когда все остановки там уже есть
    //Это избавляет нас от проблемы "указано расстояние до остановки, которая дальше во входных данных
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




} //namespace transport_catalogue::serialize
