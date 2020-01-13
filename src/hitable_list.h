#ifndef HITABLELISTH
#define HITABLELISTH

#include "hitable.h"

class hitable_list : public Hitable {
 public:
  hitable_list() {}
  hitable_list(Hitable** l, int n) {
    list = l;
    list_size = n;
  }
  virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const;
  virtual float pdf_value(const vec3& o, const vec3& v) const {
    float sum = 0.f;
    for (int i = 0; i < list_size; i++) {
      sum += list[i]->pdf_value(o, v);
    }
    return sum / list_size;
  }

  virtual vec3 random(const vec3& o) const {
    int randInd = rand() % list_size;
    return list[randInd]->random(o);
  }

 private:
  Hitable** list;
  int list_size;
};
#endif
