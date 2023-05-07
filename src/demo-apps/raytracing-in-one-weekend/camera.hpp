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
        double aspect_ratio,
        double aperture,
        double focus_dist
    ) {
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        cam_direction = unit_vector(lookfrom - lookat);
        cam_right = unit_vector(cross(vup, cam_direction));
        cam_up = cross(cam_direction, cam_right);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * cam_right;
        vertical = focus_dist * viewport_height * cam_up;
        lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*cam_direction;

        lens_radius = aperture / 2;
    }

    ray get_ray(double u, double v) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = cam_right * rd.x() + cam_up * rd.y();

        return ray(origin + offset, lower_left_corner + u*horizontal + v*vertical - origin - offset);
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 cam_direction;
    vec3 cam_right;
    vec3 cam_up;
    double lens_radius;
};