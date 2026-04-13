#include "rectangle.h"
#include <algorithm>
#include <stdexcept>

Rectangle::Rectangle(const Point& leftBottom, const Point& rightTop)
    : leftBottom_(leftBottom), rightTop_(rightTop) {
    if (leftBottom.x >= rightTop.x || leftBottom.y >= rightTop.y) {
        throw std::invalid_argument("Left bottom point must be to the left and below right top point");
    }
}

double Rectangle::getArea() const {
    double width = rightTop_.x - leftBottom_.x;
    double height = rightTop_.y - leftBottom_.y;
    return width * height;
}

Point Rectangle::getCenter() const {
    Point center;
    center.x = (leftBottom_.x + rightTop_.x) / 2.0;
    center.y = (leftBottom_.y + rightTop_.y) / 2.0;
    return center;
}

void Rectangle::move(double dx, double dy) {
    leftBottom_.x += dx;
    leftBottom_.y += dy;
    rightTop_.x += dx;
    rightTop_.y += dy;
}

void Rectangle::scale(double factor) {
    if (factor <= 0) {
        throw std::invalid_argument("Scale factor must be positive");
    }
    Point center = getCenter();
    leftBottom_.x = center.x + (leftBottom_.x - center.x) * factor;
    leftBottom_.y = center.y + (leftBottom_.y - center.y) * factor;
    rightTop_.x = center.x + (rightTop_.x - center.x) * factor;
    rightTop_.y = center.y + (rightTop_.y - center.y) * factor;
}

std::string Rectangle::getName() const {
    return "RECTANGLE";
}
