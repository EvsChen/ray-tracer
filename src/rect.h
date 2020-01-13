#ifndef RECTH
#define RECTH

#include "hitable.h"

class xy_rect : public Hitable {
 public:
  xy_rect() {}
  xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, material* mat)
      : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat){};
  virtual bool hit(const Ray& r, float t0, float t1, HitRecord& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(vec3(x0, y0, k - 0.0001), vec3(x1, y1, k + 0.0001));
    return true;
  }
  float x0, x1, y0, y1, k;
  material* mp;
};

class xz_rect : public Hitable {
 public:
  xz_rect() {}
  xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, material* mat)
      : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat){};
  virtual bool hit(const Ray& r, float t0, float t1, HitRecord& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(vec3(x0, k - 0.0001, z0), vec3(x1, k + 0.0001, z1));
    return true;
  }

  virtual float pdf_value(const vec3& o, const vec3& v) const {
    HitRecord rec;
    if (this->hit(Ray(o, v), 0.001, FLT_MAX, rec)) {
      float area = (x1 - x0) * (z1 - z0);
      float distance_squared = rec.t * rec.t * v.squared_length();
      float cosine = fabs(dot(v, rec.normal) / v.length());
      return distance_squared / (cosine * area);
    }
    return 0.f;
  }

  virtual vec3 random(const vec3& o) const {
    vec3 random_point = vec3(x0 + random_float() * (x1 - x0), k,
                             z0 + random_float() * (z1 - z0));
    return random_point - o;
  }

  float x0, x1, z0, z1, k;
  material* mp;
};

class yz_rect : public Hitable {
 public:
  yz_rect() {}
  yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, material* mat)
      : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {}
  virtual bool hit(const Ray& r, float t0, float t1, HitRecord& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(vec3(k - 0.0001, y0, z0), vec3(k + 0.0001, y1, z1));
    return true;
  }
  float y0, y1, z0, z1, k;
  material* mp;
};
#endif
