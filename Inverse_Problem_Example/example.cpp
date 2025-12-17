/* Snowden Bust optimization problem. 

*Ground Truth*: screenshotted image from object model page

*Problem*: Recover the position of the point light used in the 
screenshot.

*Methodology*: Use adam optimizer, load mesh from .obj file, make
educated initial guess from screenshot.
*/

// external
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>

// internal
#include "imageio.h"
#include "cameras.h"
#include "scenes.h"
#include "surfaces.h"


// ========== Utility Functions ==========

// Compute L2 photometric loss between two renders
double photo_loss(const std::vector<Vec3> &im1, const std::vector<Vec3> &im2) {
    int n = im1.size();
    assert(n == im2.size());
    double diff = 0.0;
    for (int i = 0; i < n; i++) {
        diff += (im1.at(i) - im2.at(i)).length_sq();
    }
    return diff / n;
}

// Save a visualization of the photometric loss between two renders
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

int main() {

    // ========== Initialize Scene ==========
    
    Scene scene(
        std::make_shared<PerspectiveCam>(
            Vec3(-0.5, 10, 4),  // look from
            Vec3(0, 0, 4),      // look at
            1.0,                // aspect ratio
            45.0                // field of view
        )
    );
    
    // Load geometry
    TriangularMesh snowden_bust("snowden_bust.obj", nullptr);
    scene.add_surface(&snowden_bust);

    // Load lighting
    PointLight light(
        Vec3(0, 5, 15),
        Vec3(1),
        150
    );
    scene.add_light(&light);
    

    // ========== Optimization Setup ===========
    
    int WIDTH = 150;
    int HEIGHT = 149;

    // Load ground truth reference image
    int gt_width, gt_height;
    std::vector<Vec3> ground_truth = load_png_as_vec("snowden_gtxs.png", gt_width, gt_height);
    assert (gt_width == WIDTH);
    assert (gt_height == HEIGHT); 

    // Statue material - educated guess based on reference image
    Material statue_mat(
        Vec3(0.35),     // ambient coefficient
        Vec3(0.55),     // diffuse coefficient
        Vec3(0.9),      // specular coefficient
        120,            // phong exponent (shininess)
        1.0,            // index of refraction
        0.0,            // filter - no transmission
        0.6,            // reflection - noticeable mirror reflection
        Vec3(0)         // filter_color: irrelevant, no transmission
    );
    snowden_bust.material = &statue_mat;

    // Lighting parameters - to optimize
    Vec3 params(0.0, 5, 15);
    light.position = params;

    // ========== ADAM Optimization Loop ==========

    // Initialize Hyperparameters
    double alpha = 0.05;
    double beta_1 = 0.9;
    double beta_2 = 0.999;
    double pertubation = 0.5;
    double pertubation2 = 2 * pertubation;
    double epsilon = 10e-8;
    int max_iters = 10000;

    // Save Initial Guess
    std::vector<Vec3> initial_guess = scene.render(WIDTH, HEIGHT);
    double loss_curr = photo_loss(ground_truth, initial_guess);

    save_png_from_vec(initial_guess, WIDTH, HEIGHT, "initial_guess.png");
    save_png_from_vec(ground_truth, WIDTH, HEIGHT, "ground_truth.png");
    printf("Reference image was saved to ground_truth.png\n");
    printf("Initial guess (%.2f, %.2f, %.2f) was saved to initial_guess.png\n", params.x, params.y, params.z);
    printf("Initial photometric loss: %.2f\n", loss_curr);

    // ADAM Loop
    Vec3 m(0.0); // first moment vector
    Vec3 v(0.0); // second moment vector
    Vec3 grad(0.0); // store gradients for each param

    for (int iter = 1; iter <= max_iters; iter++) {
        // 1. Estimate gradients with forward finite differences
        double low_loss, high_loss;
        low_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));

        // - Perturb light x-coordinate
        light.position.x = params.x + pertubation;
        high_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad.x = (high_loss - low_loss) / pertubation;

        // - Perturb light y-coordinate
        light.position.y = params.y + pertubation;
        high_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad.y = (high_loss - low_loss) / pertubation;

        // - Perturb light z-coordinate
        light.position.z = params.z + pertubation;
        high_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad.z = (high_loss - low_loss) / pertubation;

        // 2. ADAM Iterates
        m = beta_1 * m + (1.0 - beta_1) * grad;           // Update biased first moment estimate
        v = beta_2 * v + (1.0 - beta_2) * grad.dot(grad); // Update biased second raw moment estimate

        Vec3 m_hat = m / (1.0 - pow(beta_1, iter));       // Compute bias-corrected first moment estimate
        Vec3 v_hat = v / (1.0 - pow(beta_2, iter));       // Compute bias-corrected second raw moment estimate

        params = params - alpha * m_hat / (v_hat.sqrt() + Vec3(epsilon));
        light.position = params;
        
        // 3. Diagnostic Information and Early Stopping Conditions
        printf("iter: %d\n", iter);
        if (iter % 50 == 0) {
            std::vector<Vec3> curr = scene.render(WIDTH, HEIGHT);
            save_png_from_vec(curr, WIDTH, HEIGHT, "iter" + std::to_string(iter) + ".png");
            loss_curr = photo_loss(ground_truth, curr);
            double grad_norm = grad.length();

            // Diagnostics
            printf(
                "Iter: %d  |  loss: %.2e  |  params: (%.2f, %.2f, %.2f)  |  grad_norm: %.2e\n",
                iter, 
                loss_curr,
                params.x,
                params.y,
                params.z,
                grad_norm
            );

            // Early Stopping
            if (grad_norm < 1e-4 || loss_curr < 1e-5) {
                printf("Early stopping at iteration %d\n", iter);
                break;
            }
        }
    }

    // 4. Print Final Result, Output Final Image
    std::vector<Vec3> final_estimate = scene.render(WIDTH, HEIGHT);
    save_png_from_vec(final_estimate, WIDTH, HEIGHT, "final.png");
    printf("Final params: (%.2f, %.2f, %.2f)\n", params.x, params.y, params.z);
    printf("Final estimate saved to final.png\n");
}