#pragma once

#include "surfaces/surface.h"
#include "materials.h"

class Plane : public Surface {
public:
    // ===== MEMBERS =====
    Vec3 n, p;

    // ===== CONSTRUCTORS =====
    Plane();
    Plane(Vec3 plane_normal, Vec3 plane_point, Material mat);

    // ===== METHODS =====
    struct Intersection hit(Ray ray, double t0, double t1);
};