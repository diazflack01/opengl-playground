#pragma once

#include "ray.hpp"

struct hit_record {
    point3 p; // intersection point
    vec3 normal;
    double t; // distance of intersection point from camera ray
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};