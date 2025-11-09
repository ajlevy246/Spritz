#pragma once

#include "scenes.h"
#include "raytracing.h"

class Material {
public:
    // ===== MEMBERS =====
    Vec3 ka, kd, ks; 
    int shininess;
    double ior, filter, reflection;
    Vec3 filter_color;

    // ===== CONSTRUCTORS =====
    Material();
    Material(Vec3 ka_, Vec3 kd_, Vec3 ks_, int shininess_, double ior_, double filter_, double reflection_, Vec3 filter_color);

    // ===== METHODS =====
    Vec3 blinn_phong(const Vec3& surface_normal, const Vec3& light_direction, const Vec3& viewing_direction) const;
    double shlick_approx(const Vec3& surface_normal, const Vec3& viewing_direction) const;
    Vec3 shade(const Scene& scene, const Ray& ray, const Intersection& hit, int bounce) const;
};