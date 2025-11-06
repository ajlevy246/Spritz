#pragma once

#include "raytracing.h"
#include "materials.h"
#include "surfaces.h"

class Scene; // forward declaration to avoid circular dependency;

class Light {
public: 
    // Abstract base class for Lighting
    virtual Vec3 illuminate(Scene* scene, Ray* ray, Intersection* intersection) = 0;
};