#ifndef AABBH
#define AABBH

#include <cfloat>

#include "ray.h"

// faster than built-in method since we don't consider NaN and other exceptions
inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class aabb {
 private:
  vec3 _min, _max;

 public:
  aabb() : _min(FLT_MAX), _max(FLT_MIN) {}
  aabb(const vec3& a, const vec3& b) {
    _min = a;
    _max = b;
  }
  vec3 min() const { return _min; }
  vec3 max() const { return _max; }
  vec3 getCentroid() const {
    return 0.5f * (_min + _max);
  }

  void extend(const vec3 &v) {
    _max = Max(_max, v);
    _min = Min(_min, v);
  }

  void extend(const aabb &box) {
    _max = Max(_max, box.max());
    _min = Min(_min, box.min());
  }

  float getSurfaceArea() const {
    float sideX = _max[0] - _min[0],
          sideY = _max[1] - _min[1],
          sideZ = _max[2] - _min[2];
    return 2 * (sideX * sideY + sideX * sideZ + sideY * sideZ);
  }

  // Return the max extent axis
  int getMaxExtentAxis() {
    int axis = -1;
    float maxExtent = FLT_MIN;
    for (int i = 0; i < 3; ++i) {
      if (_max[i] - _min[i] > maxExtent) {
        maxExtent = _max[i] - _min[i];
        axis = i;
      }
    }
    return axis;
  }

  bool hit(const Ray& r, float tmin, float tmax) const {
    for (int a = 0; a < 3; a++) {
      // If the direction is negative X
      float invD = 1.0f / r.direction()[a];
      float t0 = (min()[a] - r.origin()[a]) * invD;
      float t1 = (max()[a] - r.origin()[a]) * invD;
      if (invD < 0.0f) {
        std::swap(t0, t1);
      }
      tmin = ffmax(t0, tmin);
      tmax = ffmin(t1, tmax);
      if (tmax <= tmin) {
        return false;
      }
    }
    return true;
  }
};

inline aabb surrounding_box(const aabb &box0, const aabb &box1) {
  return aabb(Min(box0.min(), box1.min()), Max(box0.max(), box1.max()));
}

inline aabb surrounding_box(const aabb &box0, const vec3 &v0) {
  return aabb(Min(box0.min(), v0), Max(box0.max(), v0));
}
#endif
