#pragma once

#include "hitable.h"
#include "material.h"

class constant_medium : public Hitable {
    public:
        constant_medium(Hitable *b, float d, texture *a)
            : boundary(b), density(d) {
                phase_function = new isotropic(a);
            }
        virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const {
            return boundary->bounding_box(t0, t1, box);
        }
        Hitable *boundary;
        float density;
        material *phase_function;
};
