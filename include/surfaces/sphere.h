#pragma once

#include "surfaces/surface.h"
#include "materials.h"

class Sphere : public Surface {
public:
    // ===== MEMBERS =====
    double r;
    Vec3 c;

    // ===== CONSTRUCTORS =====
    Sphere();
    Sphere(double radius, Vec3 center, Material* mat);

    // ===== METHODS =====
    struct Intersection hit(Ray ray, double t0, double t1);
};