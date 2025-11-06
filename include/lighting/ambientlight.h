#pragma once

#include "light.h"

class AmbientLight : public Light {
public:
    // ===== MEMBERS =====
    Vec3 intensity;

    // ===== CONSTRUCTORS ===== 
    AmbientLight();
    AmbientLight(Vec3 intensity_);

    // ===== METHODS =====
    Vec3 illuminate(Scene* scene, Ray* ray, Intersection* intersection);
};