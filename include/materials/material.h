#pragma once

#include "raytracing.h"

class Material {
public:
    // ===== MEMBERS =====
    Vec3 ka, kd, ks; 
    int phong_exp;

    // ===== CONSTRUCTORS =====
    Material();
    Material(Vec3 ambient, Vec3 diffusion, Vec3 specular, int shininess);

    // ===== METHODS =====
    Vec3 reflect(Vec3 light_direction, Vec3 viewing_direction, Vec3 surface_normal);
};