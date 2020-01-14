#include "accelerators/bvh.h"

#include <algorithm>

bool BVHNode::hit(const Ray &r, float tMin, float tMax, HitRecord &rec) const {
  if (!box.hit(r, tMin, tMax)) {
    return false;
  }
  if (!left && !right) {
    // Indicate this is a leaf node
    float tMinHit = FLT_MAX;
    HitRecord minRec;
    bool hitAnything = false;
    std::vector<sPtr<Hitable>> &hitables = accelerator->hitables;
    for (int i = start; i < start + nPrimitives; ++i) {
      if (hitables[i]->hit(r, tMin, tMax, minRec)) {
        hitAnything = true;
        if (minRec.t < tMinHit) {
          rec = minRec;
          tMinHit = minRec.t;
        }
      }
    }
    return hitAnything;
  }

  HitRecord leftRec, rightRec;
  bool hitLeft = left->hit(r, tMin, tMax, leftRec);
  bool hitRight = right->hit(r, tMin, tMax, rightRec);
  // If hit both left and right, record the first hit
  if (hitLeft && hitRight) {
    rec = leftRec.t < rightRec.t ? leftRec : rightRec;
    return true;
  } else if (hitLeft || hitRight) {
    rec = hitLeft ? leftRec : rightRec;
    return true;
  }
  return false;
}

bool box_x_compare(const sPtr<Hitable> &a, const sPtr<Hitable> &b) {
  aabb box_left, box_right;
  if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right)) {
    std::cerr << "no bounding box in bvh_node constructor" << std::endl;
  }
  return box_left.min().x() < box_right.min().x();
}

bool box_y_compare(const sPtr<Hitable> &a, const sPtr<Hitable> &b) {
  aabb box_left, box_right;
  if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right)) {
    std::cerr << "no bounding box in bvh_node constructor" << std::endl;
  }
  return box_left.min().y() < box_right.min().y();
}

bool box_z_compare(const sPtr<Hitable> &a, const sPtr<Hitable> &b) {
  aabb box_left, box_right;
  if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right)) {
    std::cerr << "no bounding box in bvh_node constructor" << std::endl;
  }
  return box_left.min().z() < box_right.min().z();
}

void BVH::buildLeaf(BVHNode *node, int start, int end) {
  node->start = start;
  node->nPrimitives = end - start;
  aabb tempBox;
  for (int i = start; i < end; ++i) {
    hitables[i]->bounding_box(tMin, tMax, tempBox);
    node->box = surrounding_box(tempBox, node->box);
  }
}

void BVH::buildEqualCounts(BVHNode *node, int start, int end) {
  // If we only has one object, we duplicate it on both sides
  int n = end - start;
  node->left = mkU<BVHNode>(this);
  node->right = mkU<BVHNode>(this);
  if (n == 1) {
    buildLeaf(node->left.get(), start, end);
    buildLeaf(node->right.get(), start, end);
    node->box = surrounding_box(node->left->box, node->right->box);
    return;
  }

  aabb centerBox, tempBox;
  // TODO: Reduce the number of passes through the hitables
  // Currently the parent will loop over all the child nodes
  for (int i = start; i < end; i++) {
    hitables[i]->bounding_box(tMin, tMax, tempBox);
    centerBox = surrounding_box(centerBox, tempBox.getCentroid());
  }

  int axis = centerBox.getMaxExtentAxis();

  if (axis == 0) {
    std::sort(hitables.begin() + start, hitables.begin() + end, box_x_compare);
  } else if (axis == 1) {
    std::sort(hitables.begin() + start, hitables.begin() + end, box_y_compare);
  } else {
    std::sort(hitables.begin() + start, hitables.begin() + end, box_z_compare);
  }

  int mid = start + (end - start) / 2;
  buildEqualCounts(node->left.get(), start, mid);
  buildEqualCounts(node->right.get(), mid, end);

  node->box = surrounding_box(node->left->box, node->right->box);
}

void BVH::buildSAH(BVHNode *node, int start, int end) {
  // If we only has one object, we duplicate it on both sides
  int n = end - start;
  if (n == 1) {
    node->left = mkU<BVHNode>(this);
    node->right = mkU<BVHNode>(this);
    buildLeaf(node->left.get(), start, end);
    buildLeaf(node->right.get(), start, end);
    node->box = surrounding_box(node->left->box, node->right->box);
    return;
  }

  aabb centerBox, totalBox;
  aabb hitableBounds[end - start];
  // TODO: Reduce the number of passes through the hitables
  // Currently the parent will loop over all the child nodes
  for (int i = start; i < end; i++) {
    hitables[i]->bounding_box(tMin, tMax, hitableBounds[i - start]);
    totalBox = surrounding_box(totalBox, hitableBounds[i - start]);
    centerBox = surrounding_box(centerBox, hitableBounds[i - start].getCentroid());
  }

  int axis = centerBox.getMaxExtentAxis();
  float axisMax = centerBox.max()[axis], axisMin = centerBox.min()[axis];

  // When the hitables have the same centroid, we switch to EqualCounts
  if (axisMax == axisMin || n < 4) {
    buildEqualCounts(node, start, end);
    return;
  }

  int numOfBuckets = 12;
  struct BucketInfo {
    aabb bound;
    size_t count = 0;
  };
  BucketInfo buckets[numOfBuckets];
  for (int i = start; i < end; i++) {
    vec3 center = hitableBounds[i - start].getCentroid();
    // Calculate the relative offset from 0 to 1
    float offset = (center[axis] - axisMin) / (axisMax - axisMin);
    int bucketIdx = offset * numOfBuckets;
    buckets[bucketIdx].bound = surrounding_box(buckets[bucketIdx].bound, hitableBounds[i - start]);
    buckets[bucketIdx].count++;
  }
  // There are numOfBuckets - 1 ways to split the buckets into two piles
  float cost[numOfBuckets - 1];
  // TODO: This loop can be optimized to O(N)
  for (size_t i = 1; i < numOfBuckets; i++) {
    int leftCount = 0, rightCount = 0;
    aabb leftBound, rightBound;
    for (size_t j = 0; j < i; j++) {
      leftCount += buckets[j].count;
      leftBound = surrounding_box(leftBound, buckets[j].bound);
    }
    for (size_t j = i; j < numOfBuckets; j++) {
      rightCount += buckets[j].count;
      rightBound = surrounding_box(rightBound, buckets[j].bound);
    }
    cost[i - 1] =
        1.f + (leftBound.getSurfaceArea() * leftCount + rightBound.getSurfaceArea() * rightCount) /
                  totalBox.getSurfaceArea();
  }
  float minCost = FLT_MAX;
  int minBucketSplit = -1;
  // Find the min bucket split pos
  for (size_t i = 0; i < numOfBuckets - 1; ++i) {
    if (cost[i] < minCost) {
      minCost = cost[i];
      minBucketSplit = i + 1;
    }
  }
  // The cost for initialize all hitables to a leaf node is equal to the number of hitables
  float leafCost = end - start;
  if (leafCost < minCost) {
    buildLeaf(node, start, end);
    return;
  }
  // Split the array
  float splitBound = axisMin + static_cast<float>(minBucketSplit) / numOfBuckets * (axisMax - axisMin);
  auto midItr =
      std::partition(hitables.begin() + start, hitables.begin() + end, [&](const sPtr<Hitable> &h) {
        aabb bound;
        h->bounding_box(tMin, tMax, bound);
        return bound.getCentroid()[axis] < splitBound;
      });
  int mid = midItr - hitables.begin();
  node->left = mkU<BVHNode>(this);
  node->right = mkU<BVHNode>(this);
  buildSAH(node->left.get(), start, mid);
  buildSAH(node->right.get(), mid, end);
  node->box = surrounding_box(node->right->box, node->left->box);
}

bool BVH::hit(const Ray &r, float tMin, float tMax, HitRecord &rec) const {
  if (!root) {
    return false;
  }
  return root->hit(r, tMin, tMax, rec);
}

bool BVH::bounding_box(float tMin, float tMax, aabb &box) const {
  if (!root) {
    return false;
  }
  box = root->box;
  return true;
}

BVH::BVH(std::vector<sPtr<Hitable>> hl, float tMin, float tMax, SplitMethod sp)
    : hitables(std::move(hl)), splitMethod(sp), tMin(tMin), tMax(tMax), root(mkU<BVHNode>(this)) {
  if (hitables.size() == 0) {
    root.reset(nullptr);
    return;
  }

  switch (sp) {
    case SplitMethod::SAH: {
      buildSAH(root.get(), 0, hitables.size());
      break;
    }
    case SplitMethod::EqualCounts: {
      buildEqualCounts(root.get(), 0, hitables.size());
      break;
    }
    default: {
      break;
    }
  }
}
