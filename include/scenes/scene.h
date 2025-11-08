#pragma once

#include <vector>
#include <memory> //Smart pointers

#include "cameras.h"
#include "lighting.h"
#include "raytracing.h"
#include "surfaces.h"

const int MAX_BOUNCES = 2;

class Scene {
public:
    // ===== MEMBERS =====
    std::shared_ptr<Camera> cam;
    std::vector<Surface*> objects;
    std::vector<Light*> lights;
    float* background_data; // loaded from .hdr file
    int max_bounces;
    
    // ===== CONSTRUCTORS =====
    Scene();
    Scene(std::shared_ptr<Camera> camera);

    // ===== METHODS =====
    void load_background(const std::string  &filename);
    void add_surface(Surface* new_object);
    void add_light(Light* new_light);
    void set_camera(std::shared_ptr<Camera> new_camera);

    Vec3 project_background(const Ray& ray) const;
    Intersection hit(const Ray& ray, const double t0, const double t1) const;
    Vec3 shade_ray(const Ray& ray, int bounce) const;
    std::vector<Vec3> render(int width, int height) const;
    
private:
    int bg_width = 0;
    int bg_height = 0;
    int bg_channels = 0;
};

// class SurfaceGroup : public Surface {
// public:
//     // ===== MEMBERS =====
//     std::vector<Surface*> surfaces;

//     // ===== CONSTRUCTORS =====
//     SurfaceGroup() = default;
//     SurfaceGroup(const std::vector<Surface*>& init_surfaces) : surfaces(init_surfaces) {}

//     // ===== METHODS =====
//     void add_surface(Surface* s) { surfaces.push_back(s); }

//     Intersection hit(Ray ray, double t0, double t1) {
//         Intersection closest; // default t == INFINITY means no hit
//         for (Surface* s : surfaces) {
//             Intersection curr = s->hit(ray, t0, t1);
//             if (curr.t == INFINITY) continue;
//             closest = curr;
//             t1 = closest.t;
//         }
//         return closest;
//     }    
// };

