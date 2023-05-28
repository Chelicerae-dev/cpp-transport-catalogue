#include "map_renderer.h"

namespace transport_catalogue ::render {

    ColorIterator::ColorIterator(std::vector<svg::Color>& palette)
        : color_palette_(&palette) {
    }

    svg::Color ColorIterator::Iterate() {
        svg::Color result = *color_iterator_;
        ++color_iterator_;
        if(color_iterator_ == color_palette_->end()) {
            color_iterator_ = color_palette_->begin();
        }
        return result;
    }

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

    MapRenderer::MapRenderer(const RenderSettings& render_settings)
        : render_settings_(render_settings) {
    }

    void MapRenderer::SetCoordinates(const std::vector<detail::BusCoordinates>& buses, const std::vector<detail::Stop*>& stops) {
        //должен быть менее затратный способ создать SphereProjector, но вложенные векторы не хранятся последовательно
        std::vector<geo::Coordinates> all_coords;
        //используем отановки чтобы избежать дублирования координат для инициализации
        for(const detail::Stop* stop : stops) {
            all_coords.push_back(stop->location);
        }
        SphereProjector projector(all_coords.begin(), all_coords.end(), render_settings_.width, render_settings_.height, render_settings_.padding);

        //делаем координаты для всех маршрутов
        for(const auto& [bus, stops] : buses) {
            if(bus->stops.size() != 0) {
                std::vector<svg::Point> temp;
                for(geo::Coordinates loc : stops) {
                    temp.push_back(projector(loc));
                }
                bus_coords_.push_back({bus, temp});
            }
        }
        //делаем координаты всех остановок
        for(const detail::Stop* stop : stops) {
            stop_coords_[stop->name] = projector(stop->location);
        }
    }

    void MapRenderer::Render() {
        //Рендерим автобусы
        ColorIterator palette(render_settings_.color_palette);
        //сохраняем названия маршрутов для 2 слоя
        std::vector<svg::Text> BusNames;
        for(const SvgBusCoordinates bus : bus_coords_) {
            //определяем цвет
            svg::Color color = palette.Iterate();
            //добавляем сразу в документ кривую автобуса
            canvas_.Add(BusPoly(bus.coords, color));
            //добавляем подложку и название маргрута для 1-й точки
            BusNames.push_back(BusName(bus.bus->name, bus.coords[0], color, true));
            BusNames.push_back(BusName(bus.bus->name, bus.coords[0], color, false));
            //проверяем нужно ли второе название маршрута
            if(!bus.bus->is_looped && (bus.coords[0] != bus.coords[bus.coords.size() - bus.bus->stops.size()])) {
                BusNames.push_back(BusName(bus.bus->name, bus.coords[bus.coords.size() - bus.bus->stops.size()], color, true));
                BusNames.push_back(BusName(bus.bus->name, bus.coords[bus.coords.size() - bus.bus->stops.size()], color, false));
            }
        }
        //добавляем в документ 2й слой
        for(const auto& name : BusNames) {
            canvas_.Add(name);
        }

        //Рендерим 3-й слой - точки остановок сразу в документ
        for(const auto& [name, loc] : stop_coords_) {
            canvas_.Add(Stop(loc));
        }

        //Рендерим 4-й слой - названия остановок, сразу в документ
        //а вот можно было бы из все сразу в документ добавлять, а не слоями - можно было бы не перебирать лишний раз
        for(const auto& [name, loc] : stop_coords_) {
            canvas_.Add(StopName(name, loc, true));
            canvas_.Add(StopName(name, loc, false));
        }
    }

    void MapRenderer::Print(std::ostream& os) {
        Render();
        canvas_.Render(os);
    }

    svg::Polyline MapRenderer::BusPoly(const std::vector<svg::Point>& coords, const svg::Color& color) {
        svg::Polyline result;
        result.SetStrokeColor(color);
        result.SetFillColor(svg::NoneColor);
        result.SetStrokeWidth(render_settings_.line_width);
        result.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        result.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        for(const svg::Point& point : coords) {
            result.AddPoint(point);
        }
        return result;
    }

    svg::Text MapRenderer::BusName(const std::string& name, const svg::Point& loc, const svg::Color& color, bool is_underlayer) {
        svg::Text result;
        result.SetPosition(loc);
        result.SetOffset(render_settings_.bus_label_offset);
        result.SetFontSize(render_settings_.bus_label_font_size);
        result.SetFontFamily("Verdana");
        result.SetFontWeight("bold");
        result.SetData(name);
        if(is_underlayer) {
            result.SetFillColor(render_settings_.underlayer_color);
            result.SetStrokeColor(render_settings_.underlayer_color);
            result.SetStrokeWidth(render_settings_.underlayer_width);
            result.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            result.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        } else {
            result.SetFillColor(color);
        }
        return result;
    }

    svg::Circle MapRenderer::Stop(const svg::Point& point) {
        svg::Circle result;
        result.SetCenter(point);
        result.SetRadius(render_settings_.stop_radius);
        result.SetFillColor("white");
        return result;
    }

    svg::Text MapRenderer::StopName(const std::string& name, const svg::Point& point, bool is_underlayer) {
        svg::Text result;
        result.SetPosition(point);
        result.SetOffset(render_settings_.stop_label_offset);
        result.SetFontSize(render_settings_.stop_label_font_size);
        result.SetFontFamily("Verdana");
        result.SetData(name);
        if(is_underlayer) {
            result.SetFillColor(render_settings_.underlayer_color);
            result.SetStrokeColor(render_settings_.underlayer_color);
            result.SetStrokeWidth(render_settings_.underlayer_width);
            result.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            result.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        } else {
            result.SetFillColor("black");
        }
        return result;
    }

}

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
