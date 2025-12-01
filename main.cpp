#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "cameras.h"
#include "scenes.h"

using namespace std::chrono;

void save_png(const std::vector<Vec3>& pixels, int width, int height, const std::string& filename) {

    // Create buffer for RGB values (3 channels)
    std::vector<unsigned char> buffer(width * height * 3);

    // Convert floating-point colors to bytes
    for (int i = 0; i < width * height; ++i) {
        Vec3 color = pixels[i];
        buffer[i * 3] = static_cast<unsigned char>(std::clamp(color.x, 0.0, 1.0) * 255);
        buffer[i * 3 + 1] = static_cast<unsigned char>(std::clamp(color.y, 0.0, 1.0) * 255);
        buffer[i * 3 + 2] = static_cast<unsigned char>(std::clamp(color.z, 0.0, 1.0) * 255);
    }
    
    // Write PNG file (3 channels, RGB)
    int success = stbi_write_png(filename.c_str(), width, height, 3, buffer.data(), width * 3);
    
    if (!success) {
        throw std::runtime_error("Failed to write PNG file: " + filename);
    }
}

// ============================== Helper Materials ================================
Material gold(
    Vec3(0.24725, 0.1995, 0.0745),
    Vec3(0.75164, 0.60648, 0.22648),
    Vec3(0.628281, 0.555802, 0.366065),
    51,
    1.0,
    0.0,
    0.6,
    Vec3(1.0, 1.0, 1.0)
);
Material matte_clay(
    Vec3(0.2, 0.15, 0.1),
    Vec3(0.7, 0.5, 0.3),
    Vec3(0.05, 0.05, 0.05),
    2,
    1.0,
    0.0,
    0.0,
    Vec3(1.0, 1.0, 1.0)
);
    Material green_glass(
    Vec3(0.0, 0.0, 0.0),
    Vec3(0.0, 0.1, 0.0),
    Vec3(0.9, 0.9, 0.9),
    100,
    1.52,
    0.9,
    0.05,
    Vec3(0.1, 1.0, 0.1)
);
Material amber_glass(
    Vec3(0.0, 0.0, 0.0),
    Vec3(0.1, 0.07, 0.0),
    Vec3(0.9, 0.9, 0.9),
    100,
    1.52,
    0.85,
    0.05,
    Vec3(1.0, 0.7, 0.2)
);
Material red_plastic(
    Vec3(0.1, 0.0, 0.0),
    Vec3(0.7, 0.1, 0.1),
    Vec3(0.5, 0.5, 0.5),
    32,
    1.46,
    0.0,
    0.05,
    Vec3(1.0, 1.0, 1.0)
);

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

// ================= Inverse Graphics - Simple Gradient Descent ===============
int main() {
    int HEIGHT = 500;
    int WIDTH = 500;

    // ======================== Ground Truth Image ============================
    Scene scene(
        std::make_shared<PerspectiveCam>(
            Vec3(0, 6, 4),
            Vec3(0),
            1.0,
            114.0
        )
    );
    scene.load_background("backgrounds/night.hdr");

    Cuboid floor = Cuboid(
        Vec3(-5, -5, -1),
        Vec3(5, 5, 1),
        &matte_clay
    );
    Sphere gold_ball = Sphere(
        1,
        Vec3(-2, -2, 1.5),
        &gold
    );
    Sphere plastic_ball = Sphere(
        1,
        Vec3(0, 0, 1.5),
        &red_plastic
    );
    Cuboid prism_a = Cuboid(
        Vec3(2.5, -1, 0),
        Vec3(4, 2, 2.5),
        &amber_glass
    );
    Cuboid prism_b = Cuboid(
        Vec3(-1, 2.5, 0),
        Vec3(2, 4, 2.5),
        &green_glass
    );
    scene.add_surface(&floor);
    scene.add_surface(&gold_ball);
    scene.add_surface(&plastic_ball);
    scene.add_surface(&prism_a);
    scene.add_surface(&prism_b);

    PointLight sun = PointLight( // Set initial light position
        Vec3(5, 0, 7),
        Vec3(1),
        75
    );
    scene.add_light(&sun);

    std::vector<Vec3> ground_truth = scene.render(WIDTH, HEIGHT);
    save_png(ground_truth, WIDTH, HEIGHT, "opt/ground_truth.png");
    std::cout << "ground truth saved to /ground_truth.png" << std::endl;

    // ========================= GRADIENT DESCENT ALG =========================
    
    // Parameters - start with simple angle of light position (w/ fixed radius) - proof of concept
    double epsilon = 0.1;
    int max_iters = 10000;
    double alpha_bar = 5;
    
    // Initial guesses - GT is (5, 0, 7)
    double x_k, y_k, z_k; 
    x_k = 0;
    y_k = 2;
    z_k = 9;

    sun.position = Vec3(
        x_k, y_k, z_k
    );
    std::vector<Vec3> initial_guess = scene.render(WIDTH, HEIGHT);
    save_png(initial_guess, WIDTH, HEIGHT, "opt/initial_guess.png");
    printf("initial guess (%.2f, %.2f, %.2f) was saved to opt/initial_guess.png\n", x_k, y_k, z_k);

    // Start with basic gradient descent with fixed step size
    double low_est, high_est, grad;
    for (int i = 0; i < max_iters; i++) {
        // estimate gradient with finite differences for each param
        sun.position = Vec3(
            x_k - epsilon,
            y_k,
            z_k
        );
        low_est = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));

        sun.position = Vec3(
            x_k + epsilon,
            y_k,
            z_k
        );
        high_est = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad = (high_est - low_est) / (epsilon * 2);

        //update iterate
        x_k = x_k - alpha_bar * grad;

        // estimate gradient with finite differences for each param
        sun.position = Vec3(
            x_k,
            y_k - epsilon,
            z_k
        );
        low_est = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));

        sun.position = Vec3(
            x_k,
            y_k + epsilon,
            z_k
        );
        high_est = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad = (high_est - low_est) / (epsilon * 2);

        //update iterate
        y_k = y_k - alpha_bar * grad;

                // estimate gradient with finite differences for each param
        sun.position = Vec3(
            x_k,
            y_k,
            z_k - epsilon
        );
        low_est = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));

        sun.position = Vec3(
            x_k,
            y_k,
            z_k + epsilon
        );
        high_est = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad = (high_est - low_est) / (epsilon * 2);

        //update iterate
        z_k = z_k - alpha_bar * grad;

        // diagnostic information
        if (i % 100 == 0) {
            sun.position = Vec3(
                x_k,
                y_k,
                z_k
            );
            save_png(scene.render(WIDTH, HEIGHT), WIDTH, HEIGHT, "opt/iter" + std::to_string(i) + ".png");
            printf("iter %d\tpos: (%.2f, %.2f, %.2f)\tgrad: %.3f\tdiff: %.3f\n", i, x_k, y_k, z_k, grad, low_est);
        }
    }

    // print final result, save to png
    std::vector<Vec3> final_estimate = scene.render(WIDTH, HEIGHT);
    save_png(final_estimate, WIDTH, HEIGHT, "opt/final.png");
    printf("Final param: (%.2f, %.2f, %.2f)", x_k, y_k, z_k);
    std::cout << "Final estimate saved to final.png" << std::endl;
}