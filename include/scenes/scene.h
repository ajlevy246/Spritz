#pragma once

#include <vector>
#include <memory> //Smart pointers

#include "cameras.h"
#include "lighting.h"
#include "materials.h"
#include "raytracing.h"
#include "surfaces.h"

class Scene {
public:
    // ===== MEMBERS =====
    std::shared_ptr<Camera> cam;
    std::vector<Surface*> objects;
    std::vector<Light*> lights;
    Vec3 background;
    int max_bounces;
    
    // ===== CONSTRUCTORS =====
    Scene();
    Scene(std::shared_ptr<Camera> camera, Vec3 background_color, int max_reflections);

    // ===== METHODS =====
    void add_surface(Surface* new_object);
    void add_light(Light* new_light);
    void set_camera(std::shared_ptr<Camera> new_camera);
    Intersection hit(Ray* ray, double t0, double t1);
    Vec3 shade_ray(Ray* ray, int bounce);
    std::vector<Vec3> render(int width, int height);
    
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

