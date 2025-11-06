#include "lighting.h"
#include "scenes.h"

// ===== CONSTRUCTORS ======
PointLight::PointLight() {
    center = Vec3(0, 0, 1);
    intensity = Vec3(0.2, 0.2, 0.2);
}
PointLight::PointLight(Vec3 center_, Vec3 intensity_) {
    center = center_;
    intensity = intensity_;
}

// ===== METHODS =====
Vec3 PointLight::illuminate(Scene* scene, Ray* ray, Intersection* intersection) {
    //Apply Lambert's cosine law
    Vec3 x = ray->evaluate(intersection->t); // point of intersection
    Vec3 light_ray = center - x; // vector from the point light to the point of intersection
    double dist = light_ray.length();
    light_ray = light_ray / dist;

    Vec3 normal = intersection->normal;
    double ndotl = normal.dot(light_ray);
    if (ndotl <= 0) { return Vec3(0, 0, 0); } // Light makes no contribution

    // Check if object is in shadow
    Vec3 shadow_origin = x + normal * EPS;
    Ray shadow_ray = Ray(shadow_origin, light_ray);
    Intersection shadow_hit = scene->hit(&shadow_ray, 0, dist - EPS);
    if (shadow_hit.t != INFINITY) { return Vec3(0, 0, 0); } // Point of intersection is in shadow
    Vec3 radiance = intensity / dist / dist; // Inverse square law 
    Vec3 viewing_direction = ray->d * -1;
    Vec3 surface_reflection = intersection->surface->material.reflect(light_ray, viewing_direction, intersection->normal); 
    return radiance * surface_reflection;
}