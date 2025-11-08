#include <algorithm>

#include "materials.h"

// ----- CONSTRUCTORS -----
Refractive::Refractive() {
    ka = Vec3(0.1, 0.1, 0.1);
    kd = Vec3(0.1, 0.1, 0.1);
    ks = Vec3(0.1, 0.1, 0.1);
    phong_exp = 8;
    ior = 1.333; 
    transparency = 0.9;
    transmittance = Vec3(0.85, 0.85, 0.85);
}
Refractive::Refractive(Vec3 ambient, Vec3 diffuse, Vec3 specular, int shininess, double ior_, double transparency_, Vec3 tint) {
    ka = ambient;
    kd = diffuse;
    ks = specular;
    phong_exp = shininess;
    ior = ior_;
    transparency = transparency_;
    transmittance = tint;
}

// ----- METHODS -----
Vec3 Refractive::reflect(Vec3 light_direction, Vec3 viewing_direction, Vec3 surface_normal) {
    // Fresnel reflectance (Shlick Approximation)
    float cos_i = std::clamp(viewing_direction.dot(surface_normal), -1.0, 1.0);
    float eta_i = 1;
    float eta_t=ior;
    Vec3 normal = surface_normal;

    // Quick check in case ray is inside the object
    if (cos_i > 0) {
        std::swap(eta_i, eta_t);
        Vec3 normal = normal * -1;
    }
    cos_i = fabsf(cos_i);

    // Snell's law
    float eta = eta_i / eta_t;
    float sin_t2 = eta * eta * (1 - cos_i * cos_i);

    float kr; // Fresnel reflectance fraction

    if (sin_t2 > 1) { // Total internal reflection
        kr = 1;
    } else {
        float cos_t = sqrtf(1 - sin_t2);
        float Rs = ((eta_t * cos_i) - (eta_i * cos_t)) / ((eta_t * cos_i) + (eta_i * cos_t));
        float Rp = ((eta_i * cos_i) - (eta_t * cos_t)) / ((eta_i * cos_i) + (eta_t * cos_t));
        kr = (Rs * Rs + Rp * Rp) * 0.5;
    }

    Vec3 R = ks * kr;
    Vec3 T = transmittance * (1 - kr) * transparency;
    return T + R;
}