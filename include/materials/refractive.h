#pragma once

#include "material.h"

class Refractive : public Material {
public:
    // ----- MEMBERS -----
    double ior; // Index of refraction
    double transparency; // 0 (opaque) - 1 (clear)
    Vec3 transmittance; // tint color: (1, 1, 1) - clear

    // ----- CONSTRUCTORS -----
    Refractive();
    Refractive(Vec3 ka, Vec3 kd, Vec3 ks, int shininess, double ior_, double transparency_, Vec3 tint);

    // ----- METHODS -----
    Vec3 reflect(Vec3 light_direction, Vec3 viewing_direction, Vec3 surface_normal);
};