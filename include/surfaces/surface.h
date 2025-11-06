#pragma once

#include <vector>

#include "raytracing/ray.h"
#include "raytracing/vec3.h"
#include "materials.h"

class Surface;

struct Intersection {
    // ===== MEMBERS =====
    Surface *surface;
    double t;
    Vec3 normal;

    // ===== CONSTRUCTORS =====
    Intersection() {
        t = INFINITY; // default Intersection denotes a miss
    }
    Intersection(Surface* hit_surface, double time, Vec3 intersection_normal) {
        surface = hit_surface;
        t = time;
        normal = intersection_normal;
    }
};

class Surface { // Abstract base class for all Surfaces
public:
    Material material;
    Surface() {material = Material();}
    Surface(Material mat) {material = mat;}
    virtual Intersection hit(Ray ray, double t0, double t1) = 0;
};