#pragma once

#include "vec3.hpp"

#include <iostream>
#include <algorithm>

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel = 1.0) {
    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    // Apply gamma correction by raising to the power of `1/gamma`. Below will use gamma `2`.
    auto r = std::sqrt(scale * pixel_color.x());
    auto g = std::sqrt(scale * pixel_color.y());
    auto b = std::sqrt(scale * pixel_color.z());

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * std::clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * std::clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * std::clamp(b, 0.0, 0.999)) << '\n';
}