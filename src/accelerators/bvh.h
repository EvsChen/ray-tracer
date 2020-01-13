#pragma once

#include <vector>

#include "hitable.h"
#include "smartpointerhelp.h"

enum class SplitMethod { EqualCounts, SAH };

class BVHNode;

class BVH : public Hitable {
public:
  BVH() {}
  BVH(std::vector<sPtr<Hitable>> hitables, float tMin, float tMax,
      SplitMethod sp = SplitMethod::EqualCounts);
  ~BVH() {}

  virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;
  virtual bool bounding_box(float tMin, float tMax, aabb& box) const;

  void buildLeaf(BVHNode* node, int start, int end);
  void buildSAH(BVHNode* node, int start, int end);
  void buildEqualCounts(BVHNode* node, int start, int end);

private:
  std::vector<sPtr<Hitable>> hitables;
  SplitMethod splitMethod;
  float tMin, tMax;
  uPtr<BVHNode> root;

  friend class BVHNode;
};

class BVHNode {
public:
  BVHNode(BVH* accelerator) : accelerator(accelerator), left(nullptr), right(nullptr) {}
  ~BVHNode() {}
  virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;

  aabb box;
  BVH* accelerator;
  int start, nPrimitives = 0;
  uPtr<BVHNode> left, right;
};
