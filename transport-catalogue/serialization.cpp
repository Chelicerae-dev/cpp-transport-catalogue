#include "serialization.h"

namespace transport_catalogue::serialize {

namespace {
    proto_catalogue::Stop MakeProtoStop(detail::Stop* stop,
                        int counter,
                        std::map<detail::Stop*, uint32_t>& stop_ids_in)
    {
        proto_catalogue::Stop temp;
        temp.set_name(stop->name);
        temp.set_id(counter);
        stop_ids_in[stop] = counter;
        temp.mutable_location()->set_lat(stop->location.lat);
        temp.mutable_location()->set_lng(stop->location.lng);
        return temp;
    }

    proto_catalogue::Distance MakeProtoDistances(detail::Stop* stop_from,
                            const std::unordered_map<detail::Stop*, int>& distances,
                            const std::map<detail::Stop*, uint32_t>& stop_ids)
    {

        proto_catalogue::Distance temp;
        temp.set_stop(stop_ids.at(stop_from));
        for(auto [dest, val] : distances) {
            (*temp.mutable_distances())[stop_ids.at(dest)] = val;
        }
        return temp;

    }

    proto_catalogue::Bus MakeProtoBus(detail::Bus* bus,
                        std::map<detail::Stop*, uint32_t>& stop_ids)
    {
        proto_catalogue::Bus temp;
        temp.set_name(bus->name);
        temp.set_is_looped(bus->is_looped);
        for(detail::Stop* stop : bus->stops) {
            temp.add_stops(stop_ids.at(stop));
        }
        return temp;
    }

    proto_catalogue::StopVertex MakeVertices(detail::Stop* stop,
                     const detail::StopVertices& vertices,
                     std::map<detail::Stop*, uint32_t>& stop_ids) {
        proto_catalogue::StopVertex temp;
        temp.set_id(stop_ids.at(stop));
        temp.set_wait(vertices.wait);
        temp.set_bus(vertices.bus);
        return temp;
    }

    proto_svg::Color MakeColor(const svg::Color& color) {
        proto_svg::Color result;
        if(std::holds_alternative<std::string>(color)) {
            result.set_name(std::get<std::string>(color));
        } else if(std::holds_alternative<svg::Rgba>(color)) {
            svg::Rgba rs_color = std::get<svg::Rgba>(color);
            proto_svg::Rgba temp;
            temp.set_red(rs_color.red);
            temp.set_green(rs_color.green);
            temp.set_blue(rs_color.blue);
            temp.set_opacity(rs_color.opacity);
            *result.mutable_rgba() = temp;
        } else if(std::holds_alternative<svg::Rgb>(color)) {
            svg::Rgb rs_color = std::get<svg::Rgb>(color);
            proto_svg::Rgb temp;
            temp.set_red(rs_color.red);
            temp.set_green(rs_color.green);
            temp.set_blue(rs_color.blue);
            *result.mutable_rgb() = temp;
        }
        return result;
    }


    proto_map::RenderSettings MakeRenderSettings(const render::RenderSettings& rs) {
        proto_map::RenderSettings result;
        result.set_width(rs.width);
        result.set_height(rs.height);

        result.set_padding(rs.padding);

        result.set_line_width(rs.line_width);
        result.set_stop_radius(rs.stop_radius);

        result.set_bus_label_font_size(rs.bus_label_font_size);
        result.set_bus_label_offset_x(rs.bus_label_offset.x);
        result.set_bus_label_offset_y(rs.bus_label_offset.y);

        result.set_stop_label_font_size(rs.stop_label_font_size);
        result.set_stop_label_offset_x(rs.stop_label_offset.x);
        result.set_stop_label_offset_y(rs.stop_label_offset.y);

        *result.mutable_underlayer_color() = MakeColor(rs.underlayer_color);
        result.set_underlayer_width(rs.underlayer_width);

        for(const svg::Color& color : rs.color_palette) {
            *result.add_color_palette() = MakeColor(color);
        }
        return result;
    }


    detail::Stop ParseProtoStop(const proto_catalogue::Stop& stop)
    {
//        for(auto& stop : pc.stops()) {
        return {stop.name(), {stop.location().lat(), stop.location().lng()}};

//        }
    }

    std::map<std::string, int> ParseProtoDistances(const proto_catalogue::Distance& proto_distances,
                             const std::map<uint32_t, detail::Stop*>& stop_ids)
    {
        std::map<std::string, int> distances;
        for(auto& [id, val] : proto_distances.distances()) {
            distances[stop_ids.at(id)->name] = val;
        }
        return distances;

    }

    detail::BusCreationQuery ParseProtoBus(const proto_catalogue::Bus& proto_bus, std::map<uint32_t, detail::Stop*>& stop_ids)
    {
            detail::BusCreationQuery temp;
            temp.name = proto_bus.name();
            temp.is_looped = proto_bus.is_looped();
            for(auto stop : proto_bus.stops()) {
                temp.stops.push_back(stop_ids.at(stop)->name);
            }
            return temp;
    }

    svg::Color RenderColor(const proto_svg::Color& color) {
        if(color.value_case() == 0) {
            return std::monostate();
        } else if(color.value_case() == 1) {
            return color.name();
        } else if(color.value_case() == 2) {
            svg::Rgba rgba;
            rgba.red = color.rgba().red();
            rgba.green = color.rgba().green();
            rgba.blue = color.rgba().blue();
            rgba.opacity = color.rgba().opacity();
            return rgba;
        } else if(color.value_case() == 3) {
            svg::Rgb rgb;
            rgb.red = color.rgb().red();
            rgb.green = color.rgb().green();
            rgb.blue = color.rgb().blue();
            return rgb;
        }else {
            throw std::logic_error("Invalid proto color");
        }
    }

    std::unordered_map<std::string_view, detail::StopVertices> ParseVertices(proto_catalogue::TransportCatalogue& pc,
                                                                              std::map<uint32_t, detail::Stop*>& stop_ids) {
        std::unordered_map<std::string_view, detail::StopVertices> result;
        for(auto& vertices : pc.vertices()) {
            result[stop_ids.at(vertices.id())->name] = {stop_ids.at(vertices.id()), vertices.wait(), vertices.bus()};
        }
        return result;
    }

    render::RenderSettings ParseRenderSettings(proto_catalogue::TransportCatalogue& pc) {
        render::RenderSettings result;
        //для более короткой записи получаем render_settings
        proto_map::RenderSettings rs = pc.render_settings();

        result.width = rs.width();
        result.height = rs.height();
        result.padding = rs.padding();
        result.line_width = rs.line_width();
        result.stop_radius = rs.stop_radius();

        result.bus_label_font_size = rs.bus_label_font_size();
        result.bus_label_offset.x = rs.bus_label_offset_x();
        result.bus_label_offset.y = rs.bus_label_offset_y();

        result.stop_label_font_size = rs.stop_label_font_size();
        result.stop_label_offset.x = rs.stop_label_offset_x();
        result.stop_label_offset.y = rs.stop_label_offset_y();

        result.underlayer_color = RenderColor(rs.underlayer_color());
        result.underlayer_width = rs.underlayer_width();

        for(const auto& color : rs.color_palette()) {
            result.color_palette.push_back(RenderColor(color));
        }

        return result;
    }

    proto_graph::Weight MakeProtoWeightStop(const detail::Weight& input, const std::map<std::string_view, uint32_t>& stop_ids) {
        proto_graph::Weight weight;
        weight.set_value(input.value);
        weight.set_is_wait(input.is_wait);
        weight.set_span(input.span);
        weight.set_id(stop_ids.at(input.name));
        return weight;
    }

    proto_graph::Weight MakeProtoWeightBus(const detail::Weight& input, const std::map<std::string_view, uint32_t>& bus_ids) {
        proto_graph::Weight weight;
        weight.set_value(input.value);
        weight.set_is_wait(input.is_wait);
        weight.set_span(input.span);
        weight.set_id(bus_ids.at(input.name));
        return weight;
    }

    proto_router::TransportRouter MakeProtoRouter(routing::TransportRouter& t_router,
                                            const std::map<std::string_view, uint32_t>& stopname_ids,
                                            const std::map<std::string_view, uint32_t>& bus_ids) {
        proto_router::TransportRouter result;
        auto t_router_data = t_router.GetData();
        result.mutable_settings()->set_bus_wait_time(t_router_data.first.bus_wait_time);
        result.mutable_settings()->set_bus_velocity(t_router_data.first.bus_velocity);

        //разбираем граф, дай мне сил пережить это
        auto router_ptr = t_router_data.second;
        //сначала граф, он будто бы попроще
        proto_graph::Graph proto_graph;
        auto graph = router_ptr->GetRouterData().first;
        for(auto& edge : graph.GetData().first) {
            proto_graph::Edge proto_edge;
            proto_edge.set_from(edge.from);
            proto_edge.set_to(edge.to);
            if(edge.weight.is_wait) {
                *proto_edge.mutable_weight() = MakeProtoWeightStop(edge.weight, stopname_ids);
            } else {
                *proto_edge.mutable_weight() = MakeProtoWeightBus(edge.weight, bus_ids);
            }
            *proto_graph.add_edges() = proto_edge;
        }
        for(auto& i_list : graph.GetData().second) {
            proto_graph::IncidenceList proto_list;
            for(auto incidence : i_list) {
                proto_list.add_values(incidence);
            }
            *proto_graph.add_incidence_lists() = proto_list;
        }

        *result.mutable_router()->mutable_graph() = proto_graph;

        //теперь данные из самого роутера
        auto internal_router_data = router_ptr->GetRouterData().second;
        for(auto& opt_vector : internal_router_data) {
            proto_graph::OptionalVector proto_o_v;
            for(auto opt_data : opt_vector) {
                if(opt_data.has_value() && opt_data.value().weight.name != "") {
                    proto_graph::OptionalData proto_o_d;
                    proto_graph::RouteInternalData internal_data;
                    if(opt_data.value().prev_edge.has_value()) {
                        internal_data.set_edge(opt_data.value().prev_edge.value());
                    }
                    if(opt_data.value().weight.is_wait) {
                        *internal_data.mutable_weight() = MakeProtoWeightStop(opt_data.value().weight, stopname_ids);
                    } else {
                        *internal_data.mutable_weight() = MakeProtoWeightBus(opt_data.value().weight, bus_ids);
                    }
                    *proto_o_d.mutable_route_internal_data() = internal_data;
                    *proto_o_v.add_data() = proto_o_d;
                } else {
                    proto_o_v.add_data();
                }
            }
            *result.mutable_router()->add_routes_internal_data() = proto_o_v;
        }
        //Вроде бы заполнили роутер
        return result;
    }



    //Парсим элементы графа для сборки transport_router
    detail::Weight MakeWeight(const proto_graph::Weight& pw,
                              const std::map<uint32_t, detail::Stop*>& stop_ids,
                              const std::vector<detail::Bus*>& buses) {
        detail::Weight result;
        if(pw.is_wait()) {
            result.name = stop_ids.at(pw.id())->name;
        } else {
            result.name = buses[pw.id()]->name;
        }
        result.is_wait = pw.is_wait();
        result.span = pw.span();
        result.value = pw.value();
        return result;
    }

    std::vector<graph::Edge<detail::Weight>> ParseGraphEdges(proto_catalogue::TransportCatalogue& pc, backend::TransportCatalogue& tc,
                                                             const std::map<uint32_t, detail::Stop*>& stop_ids,
                                                             const std::vector<detail::Bus*>& buses) {
        proto_graph::Graph proto_graph = pc.router().router().graph();
        std::vector<graph::Edge<detail::Weight>> result;

        for(auto proto_edge : proto_graph.edges()) {
            graph::Edge<detail::Weight> temp;
            temp.from = proto_edge.from();
            temp.to = proto_edge.to();
            temp.weight = MakeWeight(proto_edge.weight(), stop_ids, buses);
            result.push_back(temp);
        }
        return result;
    }
    std::vector<std::vector<size_t>> ParseIncidenceList(proto_catalogue::TransportCatalogue& pc) {
        std::vector<std::vector<size_t>> result;
        proto_graph::Graph proto_graph = pc.router().router().graph();

        for(auto incidence_list : proto_graph.incidence_lists()) {
            std::vector<size_t> temp;
            for(auto inc : incidence_list.values()) {
                temp.push_back(inc);
            }
            result.push_back(temp);
        }

        return result;
    }

    std::vector<std::vector<std::optional<graph::Router<detail::Weight>::RouteInternalData>>> ParseRoutesInternalData(proto_catalogue::TransportCatalogue& pc,
                                                                                                                      const std::map<uint32_t, detail::Stop*>& stop_ids,
                                                                                                                      const std::vector<detail::Bus*>& buses) {
        std::vector<std::vector<std::optional<graph::Router<detail::Weight>::RouteInternalData>>> result;
        proto_graph::Router router = pc.router().router();

        //внезапно используем типы вместе auto в надежде что это поможет не запутаться
        for(const proto_graph::OptionalVector& proto_o_v : router.routes_internal_data()) {
            std::vector<std::optional<graph::Router<detail::Weight>::RouteInternalData>> temp_vector;
            for(const proto_graph::OptionalData& proto_o_d : proto_o_v.data()) {
                if(proto_o_d.has_route_internal_data()) {
                    graph::Router<detail::Weight>::RouteInternalData temp_data;
                    temp_data.weight = MakeWeight(proto_o_d.route_internal_data().weight(), stop_ids, buses);
                    if(proto_o_d.route_internal_data().prev_edge_case() == 2) {
                        temp_data.prev_edge = proto_o_d.route_internal_data().edge();
                    }
                    temp_vector.push_back(temp_data);
                } else {
                    temp_vector.push_back({});
                }
            }
            result.push_back(temp_vector);
        }
        return result;
    }

    detail::RoutingSettings ParseRoutingSettings(proto_catalogue::TransportCatalogue& pc) {
        detail::RoutingSettings result;
        result.bus_velocity = pc.router().settings().bus_velocity();
        result.bus_wait_time = pc.router().settings().bus_wait_time();
        return result;
    }

}

    bool SerializeCatalogue(backend::TransportCatalogue& catalogue, const render::RenderSettings& render_settings, routing::TransportRouter& t_router, std::ofstream& out) {
        std::vector<detail::Stop*> stops = catalogue.GetAllStops(true);
        std::vector<detail::Bus*> buses = catalogue.GetAllBuses();
        std::unordered_map<detail::Stop*, std::unordered_map<detail::Stop*, int>> distances =  catalogue.GetAllDistances();

        //Создаём общее для всех баз (остановки, автобусы, дистанции) временное хранилище Stop* -> id
        //! и Bus* -> id - вероятно, выгоднее будет добавить автобусы при десериализации
        //! перебирать при десериализации остановки или автобусы всё равно придётся
        std::map<detail::Stop*, uint32_t> stop_ids;

        //Это мы будем сериализировать
        proto_catalogue::TransportCatalogue  pc;

        //добавляем остановки на сериализацию и заполняем stop_ids
        for(int i = 0; i < stops.size(); ++i) {
            *pc.add_stops() = MakeProtoStop(stops[i], i, stop_ids);
        }
        //...дистанции
        for(const auto& [stop, dists] : distances) {
            *pc.add_distances() = MakeProtoDistances(stop, dists, stop_ids);
        }

        //...автобусы
        for(detail::Bus* bus : buses) {
            *pc.add_buses() = MakeProtoBus(bus, stop_ids);
        }

        std::map<std::string_view, uint32_t> stopname_ids;
        std::for_each(stop_ids.begin(), stop_ids.end(), [&stopname_ids](const auto& stop) {
            stopname_ids[stop.first->name] = stop.second;
        });
        std::map<std::string_view, uint32_t> bus_ids;
        for(int i = 0; i < buses.size(); ++i) {
            bus_ids[buses[i]->name] = i;
        }

        //рендер
        *pc.mutable_render_settings() = MakeRenderSettings(render_settings);

        for(auto& [stop, vertices] : catalogue.GetVertices()) {
            *pc.add_vertices() = MakeVertices(stop, vertices, stop_ids);
        }

        *pc.mutable_router() = MakeProtoRouter(t_router, stopname_ids, bus_ids);

        return pc.SerializeToOstream(&out);
    }

    backend::TransportCatalogue DeserializeCatalogue(std::ifstream& in,
                                                     render::RenderSettings& out_render_settings,
                                                     detail::RouterSerialization& router_data,
                                                     detail::RoutingSettings& routing_settings) {
        proto_catalogue::TransportCatalogue  pc;
        backend::TransportCatalogue result;

        //создаём "обратную" мапу id -> Stop*
        std::map<uint32_t, detail::Stop*> stop_ids;
        pc.ParseFromIstream(&in);

        //добавляем остановки в transport_catalogue и заполняем stop_ids
        auto proto_stops = pc.stops();
        for(int i = 0; i < proto_stops.size(); ++i) {
            proto_catalogue::Stop proto_stop = proto_stops[i];
            result.AddStop(ParseProtoStop(proto_stop));
            //записываем ID для дальнейшей работы
            stop_ids[proto_stop.id()] = result.GetStop(proto_stop.name());
        }

        //...дистанции
        for(const auto& distances : pc.distances()) {
            result.AddStopDistances(stop_ids.at(distances.stop()), ParseProtoDistances(distances, stop_ids));
        }

        //...остановки
        for(const auto& proto_bus : pc.buses()) {
            result.AddBus(result.MakeBus(ParseProtoBus(proto_bus, stop_ids)));
        }

        result.SetVertices(ParseVertices(pc, stop_ids));

        std::vector<detail::Bus*> buses = result.GetAllBuses();

        out_render_settings = ParseRenderSettings(pc);

        router_data.routes_internal_data = ParseRoutesInternalData(pc, stop_ids, buses);
        router_data.graph_edges = ParseGraphEdges(pc, result, stop_ids, buses);
        router_data.incidence_lists = ParseIncidenceList(pc);

        routing_settings = ParseRoutingSettings(pc);

        return result;
    }


} //namespace backend
