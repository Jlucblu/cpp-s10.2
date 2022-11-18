#include "svg.h"

namespace svg {

using namespace std::literals;

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
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
}

// ---------- Polyline ------------------

// Добавляет очередную вершину к ломаной линии
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
 }

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    for (auto it = points_.begin(); it != points_.end(); ++it) {
        out << it->x << ","sv << it->y;
        if (it + 1 != points_.end()) {
            out << " "sv;
        }
    }
    out << "\""sv;
    out << "/>"sv;
 }

// ---------- Text ------------------

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
    size_ = size;
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
    for (auto [ch, spec_ch] : spec_symbols_) {
        auto founded_ch = data.find(ch);
        while (founded_ch != std::string::npos) {
            data.replace(founded_ch, 1, spec_ch);
            founded_ch = data.find(ch);
        }
    }

    data_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\"" << pos_.x << "\" y=\"" << pos_.y << "\" ";
    out << "dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\" ";
    out << "font-size=\"" << size_ << "\"";

    if (font_family_ != "") {
        out << " font-family=\"" << font_family_ << "\"";
    }
    if (font_weight_ != "") {
        out << " font-weight=\"" << font_weight_ << "\"";
    }

    out << ">" << data_ << "</text>";
}

// ---------- Document ------------------

// Добавляет в svg-документ объект-наследник svg::Object
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

// Выводит в ostream svg-представление документа
void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
    for (auto& obj : objects_) {
        obj->Render(RenderContext(out, 2, 2));
    }
    out << "</svg>"sv;
}

}  // namespace svg