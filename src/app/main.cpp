#include <iostream>

#include <pt/film.hpp>
#include <pt/objparse.hpp>

int main() {
    using namespace PT;

    auto film = Film(512, 512);

    for (unsigned y = 0; y < 512; y++)
        for (unsigned x = 0; x < 512; x++) {
            const double r = (255 * !(x & y)) / 255.0;
            const double g = (x ^ y) / 255.0;
            const double b = (x | y) / 255.0;
            film.Splat(Math::Color(r, g, b), x, y);
        }

    film.SavePNG("bong.png");
}
