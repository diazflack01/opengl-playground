#include <iostream>
#include <fstream>

#include "color.hpp"
#include "vec3.hpp"
#include "ray.hpp"

bool hit_sphere(const point3& center, double radius, const ray& r) {
    // Analytic solution - https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius*radius;
    auto discriminant = b*b - 4*a*c;
    return (discriminant > 0);
}

color ray_color(const ray& r) {
    // red if sphere is hit
    if (hit_sphere(point3(0,0,-1), 0.5, r))
        return color(1, 0, 0);


    vec3 unit_direction = unit_vector(r.direction()); // unit vector will be in range `-1.0 < val < 1.0`
    auto t = 0.5*(unit_direction.y() + 1.0); // convert unit vector length to `0 <= val <= 1`
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0); // LERP/linear interpolation/blend `(1-t) * startValue + t * endValue`
}

int main() {
    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

    // Render
    std::ofstream ofs;
    ofs.open("./image.ppm");
    ofs << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cout << "Scanlines remaining: " << j << '\n';
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width-1);
            auto v = double(j) / (image_height-1);
            auto viewport_pixel = lower_left_corner + u*horizontal + v*vertical;
            ray r(origin,  viewport_pixel - origin);
            color pixel_color = ray_color(r);
            write_color(ofs, pixel_color);
        }
    }
    std::cout << "\nDone.\n";

    return 0;
}