#pragma once

#include "vec3.hpp"
#include "ray.hpp"
#include "utils.hpp"

class camera {
public:
    camera(
        point3 lookfrom, // camera position
        point3 lookat, // camera target
        vec3   vup, // global up
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio
    ) {
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        auto cam_direction = unit_vector(lookfrom - lookat);
        auto cam_right = unit_vector(cross(vup, cam_direction));
        auto cam_up = cross(cam_direction, cam_right);

        origin = lookfrom;
        horizontal = viewport_width * cam_right;
        vertical = viewport_height * cam_up;
        lower_left_corner = origin - horizontal/2 - vertical/2 - cam_direction;
    }

    ray get_ray(double u, double v) const {
        return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};