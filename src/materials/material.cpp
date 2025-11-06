#include "materials.h"

#include <algorithm>
#include <cstdio>

// ===== CONSTRUCTORS =====
Material::Material() {
    ka = Vec3(0.1, 0.1, 0.1);
    kd = Vec3(0.1, 0.1, 0.1);
    ks = Vec3(0.1, 0.1, 0.1);
    phong_exp = 8;
}
Material::Material(Vec3 ambient, Vec3 diffusion, Vec3 specular, int shininess) {
    ka = ambient;
    kd = diffusion;
    ks = specular;
    phong_exp = shininess;
}

// ===== METHODS =====
Vec3 Material::reflect(Vec3 light_direction, Vec3 viewing_direction, Vec3 surface_normal) {
    
    // Lambertian Reflection
    Vec3 diffusion_comp = kd * std::max(0.0, surface_normal.dot(light_direction));

    // Specular (Blinn-Phong calculation)
    Vec3 half_vector = (light_direction + viewing_direction).normalized();
    Vec3 specular_comp = ks * pow(std::max(0.0, surface_normal.dot(half_vector)), phong_exp);
    
    
    return diffusion_comp + specular_comp;
}