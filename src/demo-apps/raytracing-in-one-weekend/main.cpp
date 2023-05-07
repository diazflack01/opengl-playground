#include <iostream>
#include <fstream>

#include "color.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "utils.hpp"
#include "hittable_list.hpp"
#include "camera.hpp"

color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1,1,1)); // map (-1,1) to (0, 1)
    }
    vec3 unit_direction = unit_vector(r.direction()); // unit vector will be in range `-1.0 < val < 1.0`
    auto t = 0.5*(unit_direction.y() + 1.0); // map (-1,1) to (0, 1)
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0); // LERP/linear interpolation/blend `(1-t) * startValue + t * endValue`
}

int main() {
    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;

    // World
    hittable_list world;
    world.add(std::make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(std::make_shared<sphere>(point3(0,-100.5,-1), 100));

    // Camera
    camera cam;

    // Render
    std::ofstream ofs;
    ofs.open("./image.ppm");
    ofs << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cout << "Scanlines remaining: " << j << '\n';
        for (int i = 0; i < image_width; ++i) {
            // A pixel is 1x1 in size of whatever unit, random sampling within this size
            // to get color will reduce jaggedness. Very basic antialiasing approach.
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world);
            }
            write_color(ofs, pixel_color, samples_per_pixel);
        }
    }
    std::cout << "\nDone.\n";

    return 0;
}