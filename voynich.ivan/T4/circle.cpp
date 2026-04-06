#include "circle.h"

Circle::Circle(const Point& center, double r)
: centerP(center), radius(r){};

double Circle::getArea()const{
    double area = 3.14 * radius * radius;
    return area;
}

Point Circle::getCenter()const{
    return centerP;
}

void Circle::move(const Point& delta){
    centerP.x += delta.x;
    centerP.y += delta.y;
}

void Circle::scale(double factor){
    if (factor<=0){
        throw std::invalid_argument("Factor must be positive");
    }
    radius *= factor;
}

std::string Circle::getName()const{
    return "CIRCLE";
}

