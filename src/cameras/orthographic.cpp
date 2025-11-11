#include "raytracing.h"
#include "cameras.h"


// ===== CONSTRUCTORS =====
OrthographicCam::OrthographicCam() {
    aspect = 1.0;
    
    eye = Vec3(1, 1, 1);
    Vec3 look_at = Vec3(-1, -1, -1);
    w = eye - look_at;
    u = up.cross(w).normalized();
    v = w.cross(u).normalized();
};

OrthographicCam::OrthographicCam(Vec3 look_from, Vec3 look_at, double aspect_) {
    aspect = aspect_;

    Vec3 up = Vec3(0, 0, 1);
    eye = look_from;
    w = eye - look_at;
    u = up.cross(w).normalized();
    v = w.cross(u).normalized();
}

Ray OrthographicCam::generate_ray(int x, int y, int width, int height) {
    double xn = (2.0 * x / width - 1.0) * aspect;
    double yn = (1.0 - 2.0 * y / height);
    Vec3 direction = (w * -1).normalized();
    Vec3 origin = eye + u * xn + v * yn;
    return Ray(origin, direction);
}