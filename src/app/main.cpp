#include <iostream>

#include <pt/triangle.hpp>

int main() {
    using namespace PT;

    Triangle tri = {{0, 1, 0}, {0, 0, 1}, {0, -1, 0}};

    double t          = 1.7e300;
    bool   intersects = tri.IntersectRay({-1.5, 0, 0.2}, {1, 0, 0}, t);

    std::cout << t << "\n";
    std::cout << intersects;
}