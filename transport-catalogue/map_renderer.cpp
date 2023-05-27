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

    void MapRenderer::SetCoordinates(const std::vector<std::vector<geo::Coordinates>>& coords) {
        //должен быть менее затратный способ создать SphereProjector, но вложенные векторы не хранятся последовательно
        std::vector<geo::Coordinates> all_coords;
        for(const std::vector<geo::Coordinates>& bus : coords) {
            for(const geo::Coordinates& stop_coords : bus) {
                all_coords.push_back(stop_coords);
            }
        }
        SphereProjector projector(all_coords.begin(), all_coords.end(), render_settings_.width, render_settings_.height, render_settings_.padding);
        for(const std::vector<geo::Coordinates>& bus : coords) {
            if(bus.size() != 0) {
                std::vector<svg::Point> temp;
                for(geo::Coordinates coords : bus) {
                    temp.push_back(projector(coords));
                }
                coordinates_.push_back(temp);
            }
        }
    }

    void MapRenderer::Render() {
        ColorIterator palette(render_settings_.color_palette);
        for(const std::vector<svg::Point>& bus : coordinates_) {
            canvas_.Add(BusPoly(bus, palette.Iterate()));
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

}

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
