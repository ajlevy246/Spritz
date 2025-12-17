// Cornell box visualization

#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>

#include "imageio.h"
#include "cameras.h"
#include "scenes.h"
#include "surfaces.h"

using namespace std::chrono;

// ============================ Photometric L2 Loss ===========================
double photo_loss(const std::vector<Vec3> &im1, const std::vector<Vec3> &im2) {
    int n = im1.size();
    assert(n == im2.size());
    double diff = 0.0;
    for (int i = 0; i < n; i++) {
        diff += (im1.at(i) - im2.at(i)).length_sq();
    }
    return diff / n;
}

void visualize_loss(const std::vector<Vec3> &im1, const std::vector<Vec3> &im2, int width, int height, const std::string &filename) {
    assert(im1.size() == im2.size());
    assert((int)im1.size() == width * height);

    int n = width * height;
    std::vector<Vec3> diff(n);
    
    double min_val = 1e30;
    double max_val = -1e30;

    for (size_t i = 0; i < n; i++) {
        Vec3 d = im1[i] - im2[i];
        diff[i] = d;

        min_val = std::min({min_val, d.x, d.y, d.z});
        max_val = std::max({max_val, d.x, d.y, d.z});
    }

    double range = std::max(1e-12, max_val - min_val);

    std::vector<Vec3> diff_vis(n);
    for (size_t i = 0; i < n; i++) {
        diff_vis[i] = Vec3(
            (diff[i].x - min_val) / range,
            (diff[i].y - min_val) / range,
            (diff[i].z - min_val) / range
        );
    }

    save_png_from_vec(diff_vis, width, height, filename);
}

// ================= Inverse Graphics - Simple Gradient Descent ===============
int main() {
    // ======================== Ground Truth Image ============================
    Scene scene(
        std::make_shared<PerspectiveCam>(
            Vec3(0, 3.75, 1.5),
            Vec3(0, 0, 1.5),
            1.0,
            114.0
        )
    );
    scene.load_background("backgrounds/sunrise.hdr");
    
    // Cornell Box
    Cuboid left_wall(
        Vec3(1.5, 0, 0),   // min
        Vec3(1.51, 3, 3), // max
        &green_plastic
    );
    Cuboid right_wall(
        Vec3(-1.5, 0, 0),
        Vec3(-1.49, 3, 3),
        &red_plastic
    );
    Cuboid floor(
        Vec3(-1.5, 0, -0.01),
        Vec3(1.5, 3, 0),
        &matte_clay
    );
    Cuboid ceiling(
        Vec3(-1.5, 0, 3),
        Vec3(1.5, 3, 3.01),
        &matte_clay
    );
    Cuboid back_wall(
        Vec3(-1.5, -0.01, 0),
        Vec3(1.5, 0, 3),
        &matte_clay
    );

    // Additional Geometry
    Sphere sphere_a(
        0.5,
        Vec3(0, 2.5, 0.5),
        &glass
    );
    Sphere sphere_b(
        0.5,
        Vec3(0.75, 0.55, 0.5),
        &ruby
    );
    Sphere sphere_c(
        0.5,
        Vec3(-1, 1, 0.5),
        &chrome
    );
    Triangle tri_a(
        // Vec3(-1.5, 0, 3),
        Vec3(0, 0, 3),
        Vec3(-1.5, 0, 1.5),
        Vec3(-1.5, 1, 3),
        &gold
    );

    scene.add_surface(&left_wall);
    scene.add_surface(&right_wall);
    scene.add_surface(&floor);
    scene.add_surface(&ceiling);
    scene.add_surface(&back_wall);
    scene.add_surface(&sphere_a);
    scene.add_surface(&sphere_b);
    scene.add_surface(&sphere_c);
    scene.add_surface(&tri_a);

    PointLight light_a( // Set initial light position
        Vec3(0, 15, 0),
        Vec3(1),
        90
    );
    PointLight light_b(
        Vec3(2, 5, 5),
        Vec3(1),
        45
    );
    scene.add_light(&light_a);
    scene.add_light(&light_b);

    
    // print final result, save to png
    std::vector<Vec3> im1 = scene.render(5000, 5000);
    save_png_from_vec(im1, 5000, 5000, "test1.png");

    // alter parameters (swap materials and lighting)
    sphere_a.material = &ruby;
    sphere_b.material = &chrome;
    sphere_c.material = &glass;

    light_a.position.z = 5;
    light_b.position.z = 0;

    std::vector<Vec3> im2 = scene.render(5000, 5000);
    save_png_from_vec(im2, 5000, 5000, "test2.png");

    // compute loss
    visualize_loss(im1, im2, 5000, 5000, "test_diff.png");
}