#include <iostream>

#include <pt/eigen.hpp>

int main() {
    using namespace PT::Math;

    Vector a   = {1, 0, 0};
    Matrix mat = AngleAxisMatrix(Vector::UnitZ(), PI_4);

    Vector rotated = mat * a;

    std::cout << rotated << "\n";
}