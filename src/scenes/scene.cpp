#include "scenes.h"

// #include <stdio.h>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // for loading background images

// ===== MEMBERS =====
// Camera* cam;
// std::vector<Surface*> objects;
// std::vector<Light*> lights;
// Vec3 background;
// int max_bounces;
    
// ===== CONSTRUCTORS =====
Scene::Scene() {
    cam = std::shared_ptr<PerspectiveCam>();
    background_data = nullptr;
}
Scene::Scene(std::shared_ptr<Camera> camera) {
    cam = camera;
}

// ===== METHODS =====
void Scene::load_background(const std::string &filename) {

    background_data = stbi_loadf(filename.c_str(), &bg_width, &bg_height, &bg_channels, 3);
    if (!background_data) {
        std::cerr << "Failed to load background image: " << filename << std::endl;
        exit(1);
    }

    // std::cout << "Loaded background: " << filename
    //           << " (" << bg_width << "x" << bg_height << ")" << std::endl;
}
void Scene::add_surface(Surface* new_object) {
    objects.push_back(new_object);
}
void Scene::add_light(Light* new_light) {
    lights.push_back(new_light);
}
void Scene::set_camera(std::shared_ptr<Camera> new_camera) {
    cam = new_camera;
}

Vec3 Scene::project_background(const Ray& ray) const {
    if (!background_data) {
        printf("not here!\n");
        return Vec3(0.0, 0.0, 0.0); // default black
    }

    Vec3 d = ray.d.normalized();

    // Convert to spherical coordinates
    float theta = atan2(d.y, d.x);  // [-π, π]
    float phi   = asin(d.z);        // [-π/2, π/2]

    // Convert to texture coordinates [0,1]
    float u = (theta + M_PI) / (2.0f * M_PI);
    float v = (phi + M_PI / 2.0f) / M_PI;

    // Wrap horizontally, clamp vertically
    u = fmodf(u, 1.0f);
    if (u < 0.0f) u += 1.0f;
    v = std::clamp(v, 0.0f, 1.0f);

    // Convert to pixel coordinates
    int x = std::clamp(int(u * bg_width), 0, bg_width - 1);
    int y = std::clamp(int((1.0f - v) * bg_height), 0, bg_height - 1); // flip vertically

    int index = (y * bg_width + x) * 3;

    // Gamma correct and reinhard tone map
    Vec3 loaded = Vec3(
        background_data[index + 0],
        background_data[index + 1],
        background_data[index + 2]
    );
    loaded = loaded / (Vec3(1) + loaded);
    loaded.x = pow(loaded.x, 1.0 / 2.2);
    loaded.y = pow(loaded.y, 1.0 / 2.2);
    loaded.z = pow(loaded.z, 1.0 / 2.2);

    return loaded;
}

// Simple tracing logic: check if a ray has hit an object in the scene
Intersection Scene::hit(const Ray& ray, double t0, double t1) const {
    Intersection hit = Intersection();
    for (Surface* object : objects) {
        Intersection curr = object->hit(ray, t0, t1);
        if (curr.t <= t1 && curr.t >= t0) { t1=curr.t; hit = curr; }
    }
    return hit;
}

// Main ray tracing logic. Returns the color of a single pixel
Vec3 Scene::shade_ray(const Ray& ray, int bounce) const {

    // Trace ray through the scene
    Intersection hit_obj = hit(ray, 0, INFINITY);

    if (hit_obj.t == INFINITY) { // no object in the scene was hit
        return project_background(ray);
    }

    // Calculate illumination at the point of intersection
    Vec3 shade = Vec3(0);
    shade = hit_obj.surface->material->shade(*this, ray, hit_obj, bounce);
    return shade;
    
}


// // Manual parallel rendering logic. Returns a 1d array of pixel values
std::vector<Vec3> Scene::render(int width, int height) const {
    std::vector<Vec3> pixels(width * height, Vec3(0, 0, 0));
    
    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    
    auto render_rows = [&](int start_y, int end_y) {
        for (int y = start_y; y < end_y; ++y) {
            for (int x = 0; x < width; ++x) {
                int idx = y * width + x;
                Ray ray = cam->generate_ray(x, y, width, height);
                pixels[idx] = shade_ray(ray, 0);
            }
        }
    };
    
    int rows_per_thread = height / num_threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        int start_y = i * rows_per_thread;
        int end_y = (i == num_threads - 1) ? height : (i + 1) * rows_per_thread;
        threads.emplace_back(render_rows, start_y, end_y);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return pixels;
}


// // Main rendering logic. Returns a 1d array of pixel values
// std::vector<Vec3> Scene::render(int width, int height) const {
//     std::vector pixels(width * height, Vec3(0, 0, 0)); // Initialize image to black

//     // Iterate over pixels
//     for (int y = 0; y < height; ++y) {
//         for (int x = 0; x < width; ++x) {
//             int idx = y * width + x;

//             // Generate ray throuh pixel
//             Ray ray = cam->generate_ray(x, y, width, height); 

//             // Trace ray
//             Vec3 color = shade_ray(ray, 0);
//             // Add to pixel buffer
//             pixels[idx] = color;
//         }
//     }

//     return pixels;
// }