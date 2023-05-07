#pragma once

#include "utils.hpp"
#include "vec3.hpp"

struct hit_record;
class ray;

class material {
public:
    virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
};