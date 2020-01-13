#pragma once

#include "hitable.h"
#include "material.h"
#include "accelerators/bvh.h"
#include "box.h"
#include "sphere.h"
#include "medium.h"

class Scene {
public:
    Scene();
    Hitable *world, *light;
};
