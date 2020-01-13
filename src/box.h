#pragma once

#include "hitable.h"
#include "hitable_list.h"
#include "rect.h"

class box : public Hitable {
    public:
        box() {}
        box(const vec3& p0, const vec3& p1, material *ptr);
        virtual bool hit(const Ray& r, float t0, float t1, HitRecord& rec) const;
        virtual bool bounding_box(float, float, aabb& box) const {
            box = aabb(pmin, pmax);
            return true;
        }
        vec3 pmin, pmax;
        Hitable *list_ptr;
};
