#include <iostream>
#include <fstream>

#include "color.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "utils.hpp"
#include "hittable_list.hpp"
#include "camera.hpp"

enum class DiffuseRayReflectScatterType {
    UNIT_SPHERE,
    UNIT_VECTOR_SPHERE,
    HEMISPHERE,
};

color ray_color(const ray& r, const hittable& world, int depth = 4, DiffuseRayReflectScatterType diffuseType = DiffuseRayReflectScatterType::HEMISPHERE) {
    hit_record rec;

    if (depth <= 0)
        return color{0,0,0};

    // Fix shadow acne by using 0.001
    if (world.hit(r, 0.001, infinity, rec)) {
        const auto diffuseReflectRay = [&]{
            switch (diffuseType) {
                // UNIT_SPHERE, UNIT_VECTOR_SPHERE is offset of intersection point from normal
                case DiffuseRayReflectScatterType::UNIT_SPHERE:
                    return random_in_unit_sphere();
                case DiffuseRayReflectScatterType::UNIT_VECTOR_SPHERE:
                    return random_unit_vector();
                // does not rely on intersection point
                case DiffuseRayReflectScatterType::HEMISPHERE:
                    return random_in_hemisphere(rec.normal);
            }
        }();

        point3 target = rec.p + rec.normal + diffuseReflectRay;
        // `target - rec.p`, random ray bounce from intersection point
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
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
    const int max_depth = 50;

    // World
    hittable_list world;
    world.add(std::make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(std::make_shared<sphere>(point3(0,-100.5,-1), 100));

    // Camera
    camera cam;

    // Render
    std::ofstream ofs;
    ofs.open("./image.ppm", std::ios_base::trunc);
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
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(ofs, pixel_color, samples_per_pixel);
        }
    }
    std::cout << "\nDone.\n";
    ofs.close();

    return 0;
}