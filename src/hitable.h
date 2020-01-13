#pragma once

#include "aabb.h"
#include "cfloat"
#include "ray.h"

// We do not include material.h here in case of circularity
class material;

struct HitRecord {
  float t, u, v;
  vec3 p, normal;
  material* mat_ptr;
};

class Hitable {
 public:
  Hitable() {}
  virtual bool hit(const Ray& r, float t_min, float t_max,
                   HitRecord& rec) const = 0;
  virtual float pdf_value(const vec3& origin, const vec3& direction) const {
    return 0.f;
  }
  virtual vec3 random(const vec3& origin) const { return vec3(1.f, 0.f, 0.f); }
  // TODO: Remove bouding_box method
  [[deprecated("Use the new function getBoundingBox instead")]]
  virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};

class flip_normals : public Hitable {
 public:
  flip_normals(Hitable* p) : ptr(p) {}
  virtual bool hit(const Ray& r, float t_min, float t_max,
                   HitRecord& rec) const {
    if (ptr->hit(r, t_min, t_max, rec)) {
      rec.normal = -rec.normal;
      return true;
    }
    return false;
  }
  virtual bool bounding_box(float t0, float t1, aabb& box) const {
    return ptr->bounding_box(t0, t1, box);
  }
  Hitable* ptr;
};

class translate : public Hitable {
 public:
  translate(Hitable* p, const vec3& displacement)
      : ptr(p), offset(displacement) {}
  virtual bool hit(const Ray& r, float t_min, float t_max,
                   HitRecord& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const;
  Hitable* ptr;
  vec3 offset;
};

class rotate_y : public Hitable {
 public:
  rotate_y(Hitable* p, float angle);
  virtual bool hit(const Ray& r, float t_min, float t_max,
                   HitRecord& rec) const;
  virtual bool bounding_box(float, float, aabb& box) const {
    box = bbox;
    return hasbox;
  }
  Hitable* ptr;
  float sin_theta;
  float cos_theta;
  bool hasbox;
  aabb bbox;
};
