#include "sphere.h"

// Generate a random direction from distance away pointing to some point on the
// sphere
vec3 random_to_sphere(float radius, float distance_squared) {
  float r1 = random_float();
  float r2 = random_float();
  float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
  float phi = 2 * M_PI * r1;
  float x = cos(phi) * sqrt(1 - z * z);
  float y = sin(phi) * sqrt(1 - z * z);
  return vec3(x, y, z);
}

float sphere::pdf_value(const vec3& o, const vec3& v) const {
  HitRecord rec;
  if (hit(Ray(o, v), 0.001, FLT_MAX, rec)) {
    float cos_theta_max =
        sqrt(1 - radius * radius / (center - o).squared_length());
    float solid_angle = 2 * M_PI * (1 - cos_theta_max);
    return 1 / solid_angle;
  }
  return 0.f;
}

vec3 sphere::random(const vec3& o) const {
  vec3 direction = center - o;
  float distance_squared = direction.squared_length();
  onb uvw;
  uvw.build_from_w(direction);
  return uvw.local(random_to_sphere(radius, distance_squared));
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const {
  box = aabb(center - vec3(radius, radius, radius),
             center + vec3(radius, radius, radius));
  return true;
}

bool sphere::hit(const Ray& r, float t_min, float t_max,
                 HitRecord& rec) const {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;
  // Better way of writing this?
  if (discriminant > 0) {
    float temp = (-b - sqrt(b * b - a * c)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(b * b - a * c)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

vec3 moving_sphere::center(float time) const {
  return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::bounding_box(float t0, float t1, aabb& box) const {
  aabb box0 = aabb(center(t0) - vec3(radius, radius, radius),
                   center(t0) + vec3(radius, radius, radius));
  aabb box1 = aabb(center(t1) - vec3(radius, radius, radius),
                   center(t1) + vec3(radius, radius, radius));
  box = surrounding_box(box0, box1);
  return true;
}

// replace "center" with "center(r.time())"
bool moving_sphere::hit(const Ray& r, float t_min, float t_max,
                        HitRecord& rec) const {
  vec3 oc = r.origin() - center(r.time());
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;
  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}
