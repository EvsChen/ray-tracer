#ifndef VEC3H
#define VEC3H

#include <array>
#include <cmath>
#include <iostream>

template <class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
  assert(!(hi < lo));
  return (v < lo) ? lo : (hi < v) ? hi : v;
}

inline float random_float() {
  return (float)rand() / RAND_MAX;
}

class vec3 {
 public:
  vec3() {}
  vec3(float e0, float e1, float e2) : e({e0, e1, e2}) {}
  vec3(float e) : vec3(e, e, e) {}
  float x() const { return e[0]; }
  float y() const { return e[1]; }
  float z() const { return e[2]; }
  float r() const { return e[0]; }
  float g() const { return e[1]; }
  float b() const { return e[2]; }

  const vec3 &operator+() const { return *this; }
  vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
  float operator[](int i) const { return e[i]; }
  float &operator[](int i) { return e[i]; }

  vec3 &operator+=(const vec3 &v2);
  vec3 &operator-=(const vec3 &v2);
  vec3 &operator*=(const vec3 &v2);
  vec3 &operator/=(const vec3 &v2);
  vec3 &operator*=(const float t);
  vec3 &operator/=(const float t);

  float squared_length() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }
  float length() const { return sqrt(squared_length()); }

  void make_unit_vector() {
    float k = 1.0 / length();
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
  }

  std::array<float, 3> e;
};

inline std::istream &operator>>(std::istream &is, vec3 &t) {
  is >> t.e[0] >> t.e[1] >> t.e[2];
  return is;
}

inline std::ostream &operator<<(std::ostream &os, const vec3 &t) {
  os << t.e[0] << " " << t.e[1] << " " << t.e[2];
  return os;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(float t, const vec3 &v) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3 &v, float t) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator/(vec3 v, float t) {
  return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline float dot(const vec3 &v1, const vec3 &v2) {
  return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
              -(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
              v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline vec3 &vec3::operator+=(const vec3 &v) {
  e[0] += v.e[0];
  e[1] += v.e[1];
  e[2] += v.e[2];
  return *this;
}

inline vec3 &vec3::operator-=(const vec3 &v) {
  e[0] -= v.e[0];
  e[1] -= v.e[1];
  e[2] -= v.e[2];
  return *this;
}

inline vec3 &vec3::operator*=(const vec3 &v) {
  e[0] *= v.e[0];
  e[1] *= v.e[1];
  e[2] *= v.e[2];
  return *this;
}

inline vec3 &vec3::operator/=(const vec3 &v) {
  e[0] /= v.e[0];
  e[1] /= v.e[1];
  e[2] /= v.e[2];
  return *this;
}

inline vec3 &vec3::operator*=(const float t) {
  e[0] *= t;
  e[1] *= t;
  e[2] *= t;
  return *this;
}

inline vec3 &vec3::operator/=(const float t) {
  float k = 1.0 / t;
  e[0] *= k;
  e[1] *= k;
  e[2] *= k;
  return *this;
}

inline vec3 unit_vector(vec3 v) { return v / v.length(); }

inline vec3 random_cosine_direction() {
  float r1 = random_float();
  float r2 = random_float();
  float z = sqrt(1 - r2);
  float phi = 2 * M_PI * r1;
  float x = cos(phi) * sqrt(r2);
  float y = sin(phi) * sqrt(r2);
  return vec3(x, y, z);
}

inline vec3 clamp(const vec3 &v, float low, float high) {
  vec3 clamped;
  for (int i = 0; i < 3; i++) {
    clamped[i] = clamp(v[i], low, high);
  }
  return clamped;
}

class onb {
 public:
  onb() {}
  vec3& operator[](int i) { return axis[i]; }
  vec3 operator[](int i) const { return axis[i]; }
  vec3 u() const { return axis[0]; }
  vec3 v() const { return axis[1]; }
  vec3 w() const { return axis[2]; }
  vec3 local(const vec3 &a) const {
    return a[0] * u() + a[1] * v() + a[2] * w();
  }
  void build_from_w(const vec3 &n) {
    axis[2] = unit_vector(n);
    vec3 a = fabs(axis[2][0]) > 0.9 ? vec3(0, 1, 0) : vec3(1, 0, 0);
    axis[1] = unit_vector(cross(w(), a));
    axis[0] = cross(w(), v());
  }
  std::array<vec3, 3> axis;
};

#endif
