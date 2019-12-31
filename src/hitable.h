#pragma once

#include "ray.h"
#include "aabb.h"
#include "cfloat"

// We do not include material.h here in case of circularity
class material;

struct hit_record {
    float t, u, v;
    vec3 p, normal;
    material *mat_ptr;
};

class hitable {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};

class flip_normals : public hitable {
    public:
        flip_normals(hitable *p) : ptr(p) {}
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
            if (ptr->hit(r, t_min, t_max, rec)) {
                rec.normal = -rec.normal;
                return true;
            }
            return false;
        }
        virtual bool bounding_box(float t0, float t1, aabb& box) const {
            return ptr->bounding_box(t0, t1, box);
        }
        hitable *ptr;
};

class translate : public hitable {
    public:
        translate(hitable *p, const vec3& displacement) : ptr(p), offset(displacement) {}
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;
        hitable *ptr;
        vec3 offset;
};

class rotate_y : public hitable {
    public:
        rotate_y(hitable *p, float angle);
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float, float, aabb& box) const {
            box = bbox;
            return hasbox;
        }
        hitable *ptr;
        float sin_theta;
        float cos_theta;
        bool hasbox;
        aabb bbox;
};
