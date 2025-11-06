#include "scenes.h"

#include <stdio.h>

// ===== MEMBERS =====
// Camera* cam;
// std::vector<Surface*> objects;
// std::vector<Light*> lights;
// Vec3 background;
// int max_bounces;
    
// ===== CONSTRUCTORS =====
Scene::Scene() {
    cam = std::shared_ptr<PerspectiveCam>();
    background = Vec3(0, 0, 0); // Black background color by default
    max_bounces = 1; // 1 reflection bounce by default
}
Scene::Scene(std::shared_ptr<Camera> camera, Vec3 background_color, int max_reflections) {
    cam = camera;
    background = background_color;
    max_bounces = max_reflections;
}

// ===== METHODS =====
void Scene::add_surface(Surface* new_object) {
    objects.push_back(new_object);
}
void Scene::add_light(Light* new_light) {
    lights.push_back(new_light);
}
void Scene::set_camera(std::shared_ptr<Camera> new_camera) {
    cam = new_camera;
}

// Simple tracing logic: check if a ray has hit an object in the scene
Intersection Scene::hit(Ray* ray, double t0, double t1) {
    Intersection hit = Intersection();
    for (Surface* object : objects) {
        Intersection curr = object->hit(*ray, t0, t1);
        if (curr.t <= t1 && curr.t >= t0) { t1=curr.t; hit = curr; }
    }
    return hit;
}

// Main ray tracing logic. Returns the color of a single pixel
Vec3 Scene::shade_ray(Ray* ray, int bounce) {

    // Trace ray through the scene
    Intersection hit_obj = hit(ray, 0, INFINITY);

    if (hit_obj.t == INFINITY) { // no object in the scene was hit
        return background;
    }

    // Calculate illumination at the point of intersection
    Vec3 shade = Vec3(0, 0, 0);
    for (Light* light : lights) {
        Vec3 light_contribution = light->illuminate(this, ray, &hit_obj);
        shade = shade + light_contribution;
    }
    // Calculate illumination from reflections
    if (bounce < max_bounces) {
        Vec3 reflection_origin = ray->evaluate(hit_obj.t) + hit_obj.normal * EPS; 
        Vec3 reflection_direction = ray->d - hit_obj.normal * 2 * ray->d.dot(hit_obj.normal);
        Ray reflection = Ray(reflection_origin, reflection_direction);
        Vec3 ks = hit_obj.surface->material.ks;
        shade = shade + ks * shade_ray(&reflection, bounce + 1); // recurse
    }

    return shade;
    
}

// Main rendering logic. Returns a 1d array of pixel values
std::vector<Vec3> Scene::render(int width, int height) {
    std::vector pixels(width * height, Vec3(0, 0, 0)); // Initialize image to black

    // Iterate over pixels
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;

            // Generate ray throuh pixel
            Ray ray = cam->generate_ray(x, y, width, height); 

            // Trace ray
            Vec3 color = shade_ray(&ray, 0);
            // Add to pixel buffer
            pixels[idx] = color;
        }
    }

    return pixels;
}