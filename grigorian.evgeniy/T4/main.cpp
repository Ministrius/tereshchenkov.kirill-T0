#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include "rectangle.h"
#include "circle.h"
#include "rombus.h"
#include "composite_shape.h"

void printShape(const Shape& shape) {
    Point c = shape.getCenter();
    const CompositeShape* comp = dynamic_cast<const CompositeShape*>(&shape);
    if (comp) {
        std::cout << "[COMPOSITE, (" << std::fixed << std::setprecision(2)
                  << c.x << ", " << c.y << "), " << shape.getArea() << ":\n";
        for (size_t i = 0; i < comp->getSize(); ++i) {
            std::shared_ptr<Shape> inner = comp->getShape(i);
            Point ic = inner->getCenter();
            std::cout << " " << inner->getName() << ", (" << std::fixed << std::setprecision(2)
                      << ic.x << ", " << ic.y << "), " << inner->getArea() << ",\n";
        }
        std::cout << "]\n";
    } else {
        std::cout << "[" << shape.getName() << ", (" << std::fixed << std::setprecision(2)
                  << c.x << ", " << c.y << "), " << shape.getArea() << "]\n";
    }
}

int main() {
    try {
        auto r1 = std::make_shared<Rectangle>(Point{0.0, 0.0}, Point{4.0, 2.0});
        auto c1 = std::make_shared<Circle>(Point{10.0, 10.0}, 5.0);

        auto comp = std::make_shared<CompositeShape>();
        comp->addShape(r1);
        comp->addShape(c1);

        auto r2 = std::make_shared<Rectangle>(Point{1.0, 1.0}, Point{3.0, 3.0});
        auto c2 = std::make_shared<Circle>(Point{-10.0, -10.0}, 2.0);
        auto rh1 = std::make_shared<Rombus>(Point{-5.0, -5.0}, 4.0, 6.0);
        auto rh2 = std::make_shared<Rombus>(Point{5.0, 5.0}, 2.0, 2.0);

        std::vector<std::shared_ptr<Shape>> shapes = {comp, r2, c2, rh1, rh2};

        for (const auto& s : shapes) {
            printShape(*s);
        }

        std::cout << "\n";

        for (auto& s : shapes) {
            s->scale(2.0);
        }

        for (const auto& s : shapes) {
            printShape(*s);
        }

        std::cout << "\n";
    } catch (const std::exception& e) {
        return 1;
    }
    return 0;
}
