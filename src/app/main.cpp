#include <iostream>

#include <pt/objparse.hpp>

int main() {
    using namespace PT;

    auto tris = ParseObj("./data/shuttle.obj");

    if (tris.has_value()) {
        for (auto& tri : tris.value()) {
        }
    }
}
