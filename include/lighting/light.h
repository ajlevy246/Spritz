#pragma once

#include "raytracing.h"

class Scene;

// Abstract base class for Lighting
class Light {
public: 
    virtual Vec3 direction_from(const Vec3& point) = 0;
    virtual double distance_from(const Vec3& point) = 0;
    virtual Vec3 intensity_at(const Vec3& point) = 0;
};