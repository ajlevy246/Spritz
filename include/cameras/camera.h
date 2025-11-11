#pragma once

#include "raytracing/ray.h"
#include "raytracing/vec3.h"

class Camera { // Abstract base class for all Cameras
public:    
    // ===== MEMBERS =====
    Vec3 up = Vec3(0, 0, 1);
    
    // ===== METHODS =====
    virtual Ray generate_ray(int x, int y, int width, int height) = 0;
};