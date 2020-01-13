#ifndef MATERIALH
#define MATERIALH

#include "hitable.h"
#include "pdf.h"
#include "ray.h"
#include "smartpointerhelp.h"
#include "texture.h"

inline vec3 random_in_unit_sphere() {
  vec3 p;
  do {
    p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
  } while (p.squared_length() >= 1.0);
  return p;
}

inline vec3 reflect(const vec3 &v, const vec3 &n) {
  return v - 2 * dot(v, n) * n;
}

inline float schlick(float cosine, float ref_idx) {
  float r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 *= r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}

// Snell's law to find the refracted ray
inline bool refract(const vec3 &v, const vec3 &n, float ni_over_nt,
                    vec3 &refracted) {
  vec3 uv = unit_vector(v);
  float dt = dot(uv, n);
  float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
  if (discriminant > 0) {
    refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
    return true;
  }
  return false;
}

struct scatter_record {
  Ray specular_ray;
  bool is_specular;
  vec3 attenuation;
  uPtr<pdf> pdf_ptr;
};

class material {
 public:
  // TODO: Remove pdf calculation
  virtual bool scatter(const Ray &r_in, const HitRecord &hrec,
                       scatter_record *srec) const = 0;
  virtual float scattering_pdf(const Ray &, const HitRecord &,
                               const Ray &) const {
    return 0.f;
  }
  virtual vec3 emitted(const Ray &, const HitRecord &, float, float,
                       const vec3 &) const {
    return vec3(0.f);
  }
};

class lambertian : public material {
 public:
  lambertian(texture *a) : albedo(a) {}
  float scattering_pdf(const Ray &r_in, const HitRecord &rec,
                       const Ray &scattered) const {
    float cosine = dot(rec.normal, unit_vector(scattered.direction()));
    return std::max(0.0, cosine / M_PI);
  }

  virtual bool scatter(const Ray &r_in, const HitRecord &hrec,
                       scatter_record *srec) const {
    srec->is_specular = false;
    srec->attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
    srec->pdf_ptr = mkU<cosine_pdf>(hrec.normal);
    return true;
  }

  texture *albedo;
};

class metal : public material {
 public:
  metal(const vec3 &a) : albedo(a), fuzz(1) {}
  metal(const vec3 &a, float f) : albedo(a) { fuzz = f < 1 ? f : 1; }
  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       scatter_record *srec) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    srec->specular_ray = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    srec->attenuation = albedo;
    srec->is_specular = true;
    srec->pdf_ptr = nullptr;
    return dot(srec->specular_ray.direction(), rec.normal) > 0;
  }
  vec3 albedo;
  float fuzz;
};

class dielectric : public material {
 public:
  dielectric(float ri) : ref_idx(ri) {}
  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       scatter_record *srec) const {
    vec3 outward_normal;
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    vec3 refracted;
    srec->attenuation = vec3(1.0, 1.0, 1.0);
    srec->is_specular = true;
    float ni_over_nt;
    float reflect_prob = 1.0;
    float cosine;
    if (dot(r_in.direction(), rec.normal) > 0) {
      outward_normal = -rec.normal;
      ni_over_nt = ref_idx;
      cosine = ref_idx * dot(r_in.direction(), rec.normal) /
               r_in.direction().length();
    } else {
      outward_normal = rec.normal;
      ni_over_nt = 1.0 / ref_idx;
      cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
    }
    // When reflect, the reflectivity varies with angle for real glass
    if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
      reflect_prob = schlick(cosine, ref_idx);
    }
    srec->specular_ray = drand48() < reflect_prob ? Ray(rec.p, reflected)
                                                  : Ray(rec.p, refracted);
    return true;
  }
  float ref_idx;
};

class diffuse_light : public material {
 public:
  diffuse_light(texture *a) : emit(a) {}
  virtual bool scatter(const Ray &, const HitRecord &,
                       scatter_record *srec) const {
    return false;
  }
  virtual vec3 emitted(const Ray &r_in, const HitRecord &hrec, float u,
                       float v, const vec3 &p) const {
    if (dot(r_in.direction(), hrec.normal) < 0.f) {
      return emit->value(u, v, p);
    }
    return vec3(0.f);
  }
  texture *emit;
};

class isotropic : public material {
 public:
  isotropic(texture *a) : albedo(a) {}
  virtual bool scatter(const Ray &, const HitRecord &rec,
                       scatter_record *srec) const {
    srec->specular_ray = Ray(rec.p, random_in_unit_sphere());
    srec->attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }
  texture *albedo;
};

#endif
