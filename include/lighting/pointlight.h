#pragma once

#include "light.h"

class PointLight : public Light {
public:
    // ===== MEMBERS =====
    Vec3 center, intensity;

    // ===== CONSTRUCTORS ===== 
    PointLight();
    PointLight(Vec3 center_, Vec3 intensity_);

    // ===== METHODS =====
    Vec3 illuminate(Scene* scene, Ray* ray, Intersection* intersection);
};