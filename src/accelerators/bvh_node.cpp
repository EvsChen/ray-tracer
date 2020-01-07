#include "accelerators/bvh_node.h"

bool bvh_node::bounding_box(float, float, aabb& b) const {
    b = box;
    return true;
}

bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    // Check whether the box is hit
    if (!box.hit(r, t_min, t_max)) {
        return false;
    }
    // If so, check the children
    hit_record left_rec, right_rec;
    bool hit_left = left->hit(r, t_min, t_max, left_rec);
    bool hit_right = right->hit(r, t_min, t_max, right_rec);
    // If hit both left and right, record the first hit
    if (hit_left && hit_right) {
        rec = left_rec.t < right_rec.t ? left_rec : right_rec;
        return true;
    } else if (hit_left || hit_right) {
        rec = hit_left ? left_rec : right_rec;
        return true;
    }
    return false;
}

bool box_x_compare(const hitable *a, const hitable *b) {
  aabb box_left, box_right;
  if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right)) {
    std::cerr << "no bounding box in bvh_node constructor" << std::endl;
  }
  return box_left.min().x() < box_right.min().x();
}

bool box_y_compare(const hitable *a, const hitable *b) {
  aabb box_left, box_right;
  if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right)) {
    std::cerr << "no bounding box in bvh_node constructor" << std::endl;
  }
  return box_left.min().y() < box_right.min().y();
}

bool box_z_compare(const hitable *a, const hitable *b) {
  aabb box_left, box_right;
  if (!a->bounding_box(0, 0, box_left) || !b->bounding_box(0, 0, box_right)) {
    std::cerr << "no bounding box in bvh_node constructor" << std::endl;
  }
  return box_left.min().z() < box_right.min().z();
}

bvh_node::bvh_node(std::vector<hitable*> &l, int start, int end, float time0, float time1) {
    // Randomly pick an axis to sort
    int axis = int(3*drand48());
    if (axis == 0) {
      std::sort(l.begin() + start, l.begin() + end, box_x_compare);
    }
    else if (axis == 1) {
      std::sort(l.begin() + start, l.begin() + end, box_y_compare);
    }
    else {
      std::sort(l.begin() + start, l.begin() + end, box_z_compare);
    }

    // If we only has one object, we duplicate it on both sides
    int n = end - start;
    if (n == 1) {
        left = right = l[start];
    } else if (n == 2) {
        left = l[start];
        right = l[start + 1];
    } else {
        int mid = start + (end - start) / 2;
        left = new bvh_node(l, start, mid, time0, time1);
        right = new bvh_node(l, mid, end, time0, time1);
    }
    aabb box_left, box_right;
    if (!left->bounding_box(time0, time1, box_left) ||
        !right->bounding_box(time0, time1, box_right)) {
        std::cerr << "no bounding box in bvh_node constructor\n";
    }
    box = surrounding_box(box_left, box_right);
}
