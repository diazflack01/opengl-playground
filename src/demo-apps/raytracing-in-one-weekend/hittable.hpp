#pragma once

#include "ray.hpp"

class material;

struct hit_record {
    point3 p; // intersection point
    vec3 normal;
    std::shared_ptr<material> mat_ptr;
    double t; // distance of intersection point from camera ray
    bool front_face;

    /*!
     * Always make member attribute normal pointing against the ray. This approach
     * we can determine the side of surface at the time of coloring.
     */
    void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};