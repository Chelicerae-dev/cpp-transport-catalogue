#include "serialization.h"

namespace transport_catalogue::serialize {

    bool SerializeCatalogue(backend::TransportCatalogue& catalogue,
                        const render::RenderSettings& render_settings,
                        const detail::RoutingSettings& routing_settings,
                        std::ofstream& out)
    {
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
        AddProtoStops(stops, pc, stop_ids);
        //...дистанции
        AddProtoDistances(distances, pc, stop_ids);
        //...остановки
        AddProtoBuses(buses, pc, stop_ids);
        //рендер
        AddRenderSettings(render_settings, pc);

        pc.mutable_routing_settings()->set_bus_velocity(routing_settings.bus_velocity);
        pc.mutable_routing_settings()->set_bus_wait_time(routing_settings.bus_wait_time);

        return pc.SerializeToOstream(&out);
    }

    backend::TransportCatalogue DeserializeCatalogue(std::ifstream& in, render::RenderSettings& rs_out, detail::RoutingSettings& routing_settings) {
        proto_catalogue::TransportCatalogue  pc;
        backend::TransportCatalogue result;

        //создаём "обратную" мапу id -> Stop*
        std::map<uint32_t, detail::Stop*> stop_ids;
        pc.ParseFromIstream(&in);

        //добавляем остановки в transport_catalogue и заполняем stop_ids
        ParseProtoStops(pc, result, stop_ids);
        //...дистанции
        ParseProtoDistances(pc, result, stop_ids);
        //...остановки
        ParseProtoBuses(pc, result, stop_ids);

        rs_out = ParseRenderSettings(pc);

        routing_settings.bus_velocity = pc.routing_settings().bus_velocity();
        routing_settings.bus_wait_time = pc.routing_settings().bus_wait_time();

        return result;
    }

    void AddProtoStops(const std::vector<detail::Stop*>& stops,
                        proto_catalogue::TransportCatalogue& pc,
                        std::map<detail::Stop*, uint32_t>& stop_ids)
    {
        for(int i = 0; i < stops.size(); ++i) {
            detail::Stop* stop = stops[i];
            proto_catalogue::Stop temp;
            temp.set_name(stop->name);
            temp.set_id(i);
            stop_ids[stop] = i;
            temp.mutable_location()->set_lat(stop->location.lat);
            temp.mutable_location()->set_lng(stop->location.lng);
            *pc.add_stops() = temp;
        }
    }

    void AddProtoDistances(const std::unordered_map<detail::Stop*, std::unordered_map<detail::Stop*, int>>& distances,
                            proto_catalogue::TransportCatalogue& pc,
                            const std::map<detail::Stop*, uint32_t>& stop_ids)
    {
        for(const auto& [stop, dists] : distances) {
            proto_catalogue::Distance temp;
            temp.set_stop(stop_ids.at(stop));
            for(auto [dest, val] : dists) {
                (*temp.mutable_distances())[stop_ids.at(dest)] = val;
            }
            *pc.add_distances() = temp;
        }
    }

    void AddProtoBuses(const std::vector<detail::Bus*>& buses,
                        proto_catalogue::TransportCatalogue& pc,
                        std::map<detail::Stop*, uint32_t>& stop_ids)
    {
        for(detail::Bus* bus : buses) {
            proto_catalogue::Bus temp;
            temp.set_name(bus->name);
            temp.set_is_looped(bus->is_looped);
            for(detail::Stop* stop : bus->stops) {
                temp.add_stops(stop_ids.at(stop));
            }
            *pc.add_buses() = temp;
        }
    }

    void AddRenderSettings(const render::RenderSettings& render_settings,
                           proto_catalogue::TransportCatalogue& pc) {
        *pc.mutable_render_settings() = MakeRenderSettings(render_settings);
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

    void ParseProtoStops(proto_catalogue::TransportCatalogue& pc,
                         backend::TransportCatalogue& tc, std::map<uint32_t,
                         detail::Stop*>& stop_ids)
    {
        for(auto stop : pc.stops()) {
            tc.AddStop({stop.name(), {stop.location().lat(), stop.location().lng()}});
            //записываем ID для дальнейшей работы
            stop_ids[stop.id()] = tc.GetStop(stop.name());
        }
    }

    void ParseProtoDistances(proto_catalogue::TransportCatalogue& pc,
                             backend::TransportCatalogue& tc, std::map<uint32_t,
                             detail::Stop*>& stop_ids)
    {
        for(auto elem : pc.distances()) {
            std::map<std::string, int> distances;
            for(auto [id, val] : elem.distances()) {
                distances[stop_ids.at(id)->name] = val;
            }
            tc.AddStopDistances(stop_ids.at(elem.stop()), distances);
        }
    }

    void ParseProtoBuses(proto_catalogue::TransportCatalogue& pc,
                         backend::TransportCatalogue& tc, std::map<uint32_t,
                         detail::Stop*>& stop_ids)
    {
//        for(auto bus : pc.buses()) {
//            detail::BusCreationQuery temp;
//            temp.name = bus.name();
//            temp.is_looped = bus.is_looped();
//            for(auto stop : bus.stops()) {
//                temp.stops.push_back(stop_ids.at(stop)->name);
//            }
//            tc.AddBus(tc.MakeBus(temp));
//        }
        for(auto bus : pc.buses()) {
                    detail::BusCreationQuery temp;
                    temp.name = bus.name();
                    temp.is_looped = bus.is_looped();
//                    for(auto stop : bus.stops()) {
//                        temp.stops.push_back(stop_ids.at(stop)->name);
//                    }
                    for(int i = bus.stops().size() - 1; i >= 0; i--) {
                        temp.stops.push_back(stop_ids.at(bus.stops()[i])->name);
                    }
                    tc.AddBus(tc.MakeBus(temp));
                }
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

} //namespace backend
