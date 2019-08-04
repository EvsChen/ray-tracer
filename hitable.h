#ifndef HITABLEH
#define HITABLEH

#include "ray.h"

// We do not include material.h here in case of circularity
class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

class hitable {
    public:
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif
