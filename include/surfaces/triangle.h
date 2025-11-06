#pragma once

#include "surfaces/surface.h"

class Triangle : public Surface {
public:
    // ===== MEMBERS =====
    Vec3 v1, v2, v3;
    Material material;

    // ===== CONSTRUCTORS =====
    Triangle();
    Triangle(Vec3 vert1, Vec3 vert2, Vec3 vert3, Material mat);

    // ===== METHODS =====
    struct Intersection hit(Ray ray, double t0, double t1);
};