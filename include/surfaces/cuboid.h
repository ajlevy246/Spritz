#pragma once

#include "surfaces/surface.h"
#include "materials.h"

// Axis-aligned cuboid... for now
// - Rotation transformation will come later
class Cuboid : public Surface {
public:
    // ===== MEMBERS =====
    Vec3 min_p, max_p;

    // ===== CONSTRUCTORS =====
    Cuboid(Vec3 min_, Vec3 max_, Material* mat);

    // ===== METHODS =====
    struct Intersection hit(Ray ray, double t0, double t1);
};