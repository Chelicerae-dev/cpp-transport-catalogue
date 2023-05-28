#include "svg.h"

#include <algorithm>

std::ostream& operator<<(std::ostream& out, svg::StrokeLineCap cap) {
    using namespace std::string_literals;
    switch(cap) {
        case (svg::StrokeLineCap::BUTT):
            out << "butt"s;
        break;
        case (svg::StrokeLineCap::ROUND):
            out << "round"s;
        break;
        case (svg::StrokeLineCap::SQUARE):
            out << "square"s;
        break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, svg::StrokeLineJoin join) {
    using namespace std::string_literals;
    switch(join) {
        case (svg::StrokeLineJoin::ARCS):
            out << "arcs"s;
        break;
        case (svg::StrokeLineJoin::BEVEL):
            out << "bevel"s;
        break;
        case (svg::StrokeLineJoin::MITER):
            out << "miter"s;
        break;
        case (svg::StrokeLineJoin::MITER_CLIP):
            out << "miter-clip"s;
        break;
        case (svg::StrokeLineJoin::ROUND):
            out << "round"s;
        break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, svg::Color color) {
    out << *std::visit(svg::ColorRenderer{}, color);
    return out;
}

namespace svg {

using namespace std::literals;

Rgb::Rgb(uint8_t rd, uint8_t gr, uint8_t bl)
    : red(rd), green(gr), blue(bl) {
}

Rgba::Rgba(uint8_t rd, uint8_t gr, uint8_t bl, double opa)
    : red(rd), green(gr), blue(bl), opacity(opa) {
}

// ---------- ColorRenderer ---------------
std::optional<std::string> ColorRenderer::operator()(std::monostate) {
    return "none"s;
}
std::optional<std::string> ColorRenderer::operator()(std::string str) {
    return str;
}
std::optional<std::string> ColorRenderer::operator()(Rgb rgb) {
    std::stringstream strm;
    strm << "rgb("s << std::to_string(rgb.red) << ',' << std::to_string(rgb.green) << ',' << std::to_string(rgb.blue) << ')';
    return strm.str();
}
std::optional<std::string> ColorRenderer::operator()(Rgba rgba) {
    std::stringstream strm;
    strm << "rgba("s << std::to_string(rgba.red) << ',' << std::to_string(rgba.green) << ',' << std::to_string(rgba.blue) << ',' << rgba.opacity << ')';
    return strm.str();
}



void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << '"';
    RenderAttrs(out);
    out << "/>"sv;
}

// ------------Polyline---------------
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    std::string_view temp;
    out << "<polyline points=\""sv;
    for(size_t i = 0; i < points_.size(); ++i) {
        out << points_[i].x << ',' << points_[i].y;
        if(i != points_.size() - 1) {
            out << ' ';
        }
    }
    out << '"';
    RenderAttrs(out);
    out << "/>"sv;
}

// -------------Text ----------------
// Задаёт координаты опорной точки (атрибуты x и y)
Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}

// Прочие данные и методы, необходимые для реализации элемента <text>
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text"sv;
    RenderAttrs(out);
    out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << '"';
//    if(offset_.x != 0 && offset_.y != 0) {
        out << " dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\""sv;
//    }
    out << " font-size=\""sv << font_size_ << '"';
    if(font_family_.size() != 0) {
        out << " font-family=\""sv << font_family_ << '"';
    }
    if(font_weight_.size() != 0) {
        out << " font-weight=\""sv << font_weight_ << '"';
    }

    out << '>' << data_ << "</text>"sv;
}

 // ------------------ Document -----------------------
/*
 Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
 Пример использования:
 Document doc;
 doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
*/

// Добавляет в svg-документ объект-наследник svg::Object
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.push_back(std::move(obj));
}

// Выводит в ostream svg-представление документа
void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    for(auto& object : objects_) {
        object->Render({out});
        out << ' ';
    }
    out << "</svg>"sv;
}

bool operator<(Rgb lhs, Rgb rhs) {
    return lhs.red < rhs.red || lhs.green < rhs.green || lhs.blue < rhs.blue;
}

bool operator<(Rgba lhs, Rgba rhs) {
    return lhs.red < rhs.red || lhs.green < rhs.green || lhs.blue < rhs.blue || lhs.opacity < rhs.opacity;
}

bool operator!=(Point lhs, Point rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

}  // namespace svg

