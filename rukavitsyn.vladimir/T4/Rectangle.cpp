#include "Rectangle.h"

Rectangle::Rectangle(const Point& bl, const Point& tr) :
    bottomLeft_(bl),
    topRight_(tr)
{
}

double Rectangle::getArea() const {
    double width = topRight_.x_ - bottomLeft_.x_;
    double height = topRight_.y_ - bottomLeft_.y_;
    return width * height;
}

Point Rectangle::getCenter() const {
    return Point((bottomLeft_.x_ + topRight_.x_) / 2.0,
        (bottomLeft_.y_ + topRight_.y_) / 2.0);
}

void Rectangle::move(double dx, double dy) {
    bottomLeft_.x_ += dx;
    bottomLeft_.y_ += dy;
    topRight_.x_ += dx;
    topRight_.y_ += dy;
}

void Rectangle::scale(double factor) {
    double vectorBLx = getCenter().x_ - bottomLeft_.x_;
    double vectorBLy = getCenter().y_ - bottomLeft_.y_;
    double vectorTRx = topRight_.x_ - getCenter().x_;
    double vectorTRy = topRight_.y_ - getCenter().y_;
    topRight_.x_ = getCenter().x_ + (vectorTRx * factor);
    topRight_.y_ = getCenter().y_ + (vectorTRy * factor);
    bottomLeft_.x_ = getCenter().x_ + (vectorBLx * factor);
    bottomLeft_.y_ = getCenter().y_ + (vectorBLy * factor);
}

std::string Rectangle::getName() const {
    return "Rectangle";
}

std::pair<Point, Point> Rectangle::circumscribedRectangle() const
{
    return { bottomLeft_, topRight_ };
}

void Rectangle::print(std::ostream& stream) const
{
    stream << getName() << ", Center = (" << getCenter().x_ << ", " << getCenter().y_ << "), ";
    stream << "Area = " << getArea() << ";\n";
}
