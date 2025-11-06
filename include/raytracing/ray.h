#pragma once

#include "raytracing/vec3.h"

struct Ray {
    // ===== MEMBERS =====
    Vec3 o, d;

    // ===== CONSTRUCTORS =====
    Ray() {
        o=Vec3(0, 0, 0);
        d=Vec3(0, 0, 1);
    };
    Ray(Vec3& origin, Vec3& direction) {
        o=origin; 
        d=direction.normalized();
    };

    // ===== METHODS =====
    Vec3 evaluate(double t) {
        return o + d * t;
    };
};