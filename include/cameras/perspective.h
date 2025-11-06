#pragma once

#include "camera.h"

class PerspectiveCam : public Camera {
public:
    // ===== MEMBERS =====
    Vec3 eye, u, v, w;
    double aspect, fov;
        
    // ===== CONSTRUCTORS =====
    PerspectiveCam();
    PerspectiveCam(Vec3 look_from, Vec3 look_at, double aspect, double fov);

    // ===== METHODS =====
    Ray generate_ray(int x, int y, int width, int height);
};