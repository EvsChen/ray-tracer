#pragma once
#include "geometry.h"
#include "hitable.h"

class pdf {
 public:
  virtual float value(const vec3& direction) const = 0;
  virtual vec3 generate() const = 0;
  virtual ~pdf() {}
};

class cosine_pdf : public pdf {
 public:
  cosine_pdf(const vec3& w) { uvw.build_from_w(w); }
  ~cosine_pdf() {}
  virtual float value(const vec3& direction) const {
    float cosine = dot(unit_vector(direction), uvw.w());
    return std::max(0.f, cosine);
  }
  virtual vec3 generate() const { return uvw.local(random_cosine_direction()); }

 private:
  onb uvw;
};

class hitable_pdf : public pdf {
 public:
  hitable_pdf(Hitable* p, const vec3& origin) : hit(p), origin(origin) {}
  virtual float value(const vec3& direction) const {
    return hit->pdf_value(origin, direction);
  }
  virtual vec3 generate() const { return hit->random(origin); }
  vec3 origin;
  Hitable* hit;
};

class mixture_pdf : public pdf {
 public:
  mixture_pdf(pdf* p0, pdf* p1) : p({p0, p1}) {}
  virtual float value(const vec3& direction) const {
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
  }
  virtual vec3 generate() const {
    return random_float() < 0.5 ? p[0]->generate() : p[1]->generate();
  }

 private:
  std::array<pdf*, 2> p;
};