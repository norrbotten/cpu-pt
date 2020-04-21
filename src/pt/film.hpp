#pragma once

#include <lodepng.h>
#include <vector>

#include "eigen.hpp"

namespace PT {

// TODO: maybe a specialization for small constexpr size films
// but then again, how often do you have an image that fits on the stack

/**
 * A film is basically a bitmap, but it keeps track of the number of samples
 * placed on each pixel and averages them when you get a pixel from it
 */
class Film {
    Math::Color* rgb_samples;
    size_t*      num_samples;

    size_t width, height;

  public:
    Film(size_t width_, size_t height_)
        : width(width_)
        , height(height_) {

        rgb_samples = new Math::Color[width * height];
        num_samples = new size_t[width * height];
    }

    ~Film() {
        delete[] rgb_samples;
        delete[] num_samples;
    }

    auto Width() { return this->width; }
    auto Height() { return this->height; }

    /**
     * Splats a sample onto the film.
     * This does NOT range check x/y
     */
    void Splat(const Math::Color& sample, size_t x, size_t y) {
        const auto i = x + this->width * y;
        rgb_samples[i] += sample;
        num_samples[i]++;
    }

    /**
     * Gets a sample from the film
     * This does NOT range check x/y
     */
    Math::Color Sample(size_t x, size_t y) {
        const auto i = x + this->width * y;
        const auto n = num_samples[i];
        if (n == 0)
            return Math::Color(0, 0, 0);

        return Math::Color(rgb_samples[i] / (double)n);
    }

    /**
     * Saves the film as a PNG
     */
    void SavePNG(const char* filename) {
        // id rather do this in-place, but lodepng doesnt support anything for it
        std::vector<unsigned char> image;
        image.reserve(this->width * this->height * 4);

        for (size_t y = 0; y < this->height; y++)
            for (size_t x = 0; x < this->width; x++) {
                const auto rgb = this->Sample(x, y);

                image.push_back((unsigned char)(255.0 * std::clamp(rgb.x(), 0.0, 1.0)));
                image.push_back((unsigned char)(255.0 * std::clamp(rgb.y(), 0.0, 1.0)));
                image.push_back((unsigned char)(255.0 * std::clamp(rgb.z(), 0.0, 1.0)));
                image.push_back((unsigned char)255); // alpha
            }

        lodepng::encode(filename, image, this->width, this->height);
    }
};

} // namespace PT