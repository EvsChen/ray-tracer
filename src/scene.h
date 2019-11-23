#pragma once

#include "hitable.h"
#include "material.h"
#include "bvh_node.h"
#include "box.h"
#include "sphere.h"
#include "medium.h"

class Scene {
public:
    Scene();
    hitable *world;    
};
