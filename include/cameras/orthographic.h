#pragma once

#include "camera.h"

class OrthographicCam : public Camera {
public:
    // ===== MEMBERS =====
    Vec3 eye, u, v, w;
    double aspect, fov;
        
    // ===== CONSTRUCTORS =====
    OrthographicCam();
    OrthographicCam(Vec3 look_from, Vec3 look_at, double aspect);

    // ===== METHODS =====
    Ray generate_ray(int x, int y, int width, int height);
};