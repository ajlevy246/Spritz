#include "lighting.h"
#include <cstdio>

// ===== CONSTRUCTORS =====
AmbientLight::AmbientLight() {
    intensity = Vec3(0.2, 0.2, 0.2);
}
AmbientLight::AmbientLight(Vec3 intensity_) {
    intensity = intensity_;
}

// ===== METHODS =====
Vec3 AmbientLight::illuminate(Scene* scene, Ray* ray, Intersection* intersection) {
    Material material = intersection->surface->material;
    return intensity * material.ka;
}