#pragma once

#include "hitable.h"

#include <vector>

class bvh_node : public hitable {
 public:
  bvh_node() {}
  bvh_node(std::vector<hitable*> &list, int start, int end, float time0, float time1);
  virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const;
  aabb box;

 private:
  hitable *left, *right;
};
