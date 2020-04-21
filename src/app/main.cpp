#include <pt/film.hpp>
#include <pt/objparse.hpp>

int main() {
    using namespace PT;

    constexpr unsigned width  = 1024;
    constexpr unsigned height = 1024;
    constexpr double   aspect = (double)height / (double)width;

    constexpr double fov = 0.7;

    const auto up = Math::Vector{0, 0, 1};

    const Math::Vector camForward = Math::Vector(1, 0, 0);
    const Math::Vector camLeft    = up.cross(camForward);
    const Math::Vector camUp      = camForward.cross(camLeft);

    const Math::Vector camOrigin = -camForward * 14;

    std::cout << camOrigin << "\n";

    auto film = Film(width, height);

    auto CamRay = [&](double x, double y) -> Math::Vector {
        const double x_i = 2 * (x / width) - 1;
        const double y_i = aspect * (2 * (y / height) - 1);

        auto vec = Math::Vector(camForward - fov * x_i * camLeft - fov * y_i * camUp);
        vec.normalize();

        return vec;
    };

    auto model = ParseObj("./data/shuttle.obj");

    if (!model.has_value())
        return 1;

    auto& tris = model.value();

    for (unsigned y = 0; y < height; y++)
        for (unsigned x = 0; x < width; x++) {
            const auto camray = CamRay(x, y);

            double t_min = 1e300;
            for (auto& tri : tris) {

                double t;
                bool   isect = tri.IntersectRay(camOrigin, camray, t);

                if (isect && t < t_min) {
                    t_min = t;
                }
            }

            if (t_min != 1e300) {
                film.Splat(Math::Color(1, 0, 0), x, y);
            }
        }

    film.SavePNG("bong.png");
}
