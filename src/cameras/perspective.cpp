#include "raytracing.h"
#include "cameras.h"


// ===== CONSTRUCTORS =====
PerspectiveCam::PerspectiveCam() {
    aspect = 1.0;
    fov = 114;
    
    eye = Vec3(1, 1, 1);
    Vec3 look_at = Vec3(-1, -1, -1);
    Vec3 up = Vec3(0, 0, 1);
    w = eye - look_at;
    u = up.cross(w).normalized();
    v = w.cross(u).normalized();
};

PerspectiveCam::PerspectiveCam(Vec3 look_from, Vec3 look_at, double aspect_, double fov_) {
    aspect = aspect_;
    fov = fov_;

    Vec3 up = Vec3(0, 0, 1);
    eye = look_from;
    w = eye - look_at;
    u = up.cross(w).normalized();
    v = w.cross(u).normalized();
}

Ray PerspectiveCam::generate_ray(int x, int y, int width, int height) {
    double px = (2 * (x + 0.5) / width - 1) * aspect;
    double py = (1 - 2 * (y + 0.5) / height);

    Vec3 direction = (u * px + v * py - w).normalized();
    return Ray(eye, direction);
}