/* Ill Posed Scene: optimization problem. 

*Ground Truth*: rendered image with a reflective sphere in the foreground, various object in reflection/background.

*Problem*: Recover the position of the lighting used in the scene.

*Methodology*: Use adam optimizer with central finite difference gradient estimates.
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

// UTILITY FUNCTIONS

// Calculate photometric L2 Loss between two renders
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

// ================= Inverse Graphics - Simple Gradient Descent ===============
int main() {

    // ======================== Initialize Scene ============================

    Scene scene(
        std::make_shared<PerspectiveCam>(
            Vec3(3, 0, -0.2),
            Vec3(0, 0, 0),
            1.0,
            114.0
        )
    );
    scene.load_background("cobble.hdr");
    
    // Load geometry
    Sphere reflection = Sphere(
        2,
        Vec3(0),
        &chrome // ground truth material
    );
    scene.add_surface(&reflection);

    Sphere orb_1 = Sphere(
        1,
        Vec3(3, 2, 3),
        &red_plastic
    );
    Sphere orb_2 = Sphere(
        1,
        Vec3(4, -2, 0),
        &amber_glass
    );
    Cuboid cube_1 = Cuboid(
        Vec3(1, -3, -3),
        Vec3(3, 3, -2),
        &gold
    );
    Triangle tri_mirror = Triangle(
        Vec3(-9, 0, 4),
        Vec3(-1, -3, 9),
        Vec3(-1, 3, 9),
        &copper
    );
    scene.add_surface(&orb_1);
    scene.add_surface(&orb_2);
    scene.add_surface(&cube_1);
    scene.add_surface(&tri_mirror);

    // Load lighting
    PointLight light1 = PointLight(
        Vec3(4, 0, 5),
        Vec3(1),
        75
    );
    scene.add_light(&light1);
    PointLight light2 = PointLight(
        Vec3(4, 2, -3),
        Vec3(1),
        50
    );
    scene.add_light(&light2);


    // ========== Optimization Setup ==========

    int WIDTH = 500;
    int HEIGHT = 500;

    // Render ground truth reference image
    std::vector<Vec3> ground_truth = scene.render(WIDTH, HEIGHT);
    save_png_from_vec(ground_truth, WIDTH, HEIGHT, "ground_truth.png");
    std::cout << "ground truth saved to /ground_truth.png" << std::endl;

    // Initialize goal parameters - educated guess based on rendered ground truth.
    Vec3 params(0.5, 0.95, 0.0);
    Material goal_mat(
        // params to optimize
        Vec3(params.x),     // ambient coefficient  
        Vec3(params.y),     // diffuse coefficient
        Vec3(params.z),     // specular coefficient

        // hardcoded material properties
        90,           // phong exponent (shininess)
        2.5,          // index of refraction
        0.0,          // filter - no transmission
        0.95,         // reflection - mirror reflection
        Vec3(1.0)     // filter color: irrelevant, no transmission
    );
    reflection.material = &goal_mat;
    
    // ========== ADAM Optimization Loop ==========

    // Initialize hyperparameters
    double alpha = 0.05;
    double beta_1 = 0.9;
    double beta_2 = 0.999;
    double pertubation = 0.025;
    double pertubation2 = 2 * pertubation;
    double epsilon = 10e-8;
    int max_iters = 10000;

    // Save initial guess
    std::vector<Vec3> initial_guess = scene.render(WIDTH, HEIGHT);
    double loss_curr = photo_loss(ground_truth, initial_guess);

    save_png_from_vec(initial_guess, WIDTH, HEIGHT, "initial_guess.png");
    printf("Initial guess (%.2f, %.2f, %.2f) was saved to initial_guess.png\n", params.x, params.y, params.z);
    printf("Initial photometric loss: %.2e\n", loss_curr);

    // ADAM Loop
    Vec3 m(0.0); // first moment vector
    Vec3 v(0.0); // second moment vector
    Vec3 grad(0.0); // store gradients for each param

    for (int iter = 1; iter <= max_iters; iter++) {
        
        // 1. Estimate gradients with central finite differences
        double low_loss, high_loss;
        
        // - Perturb ambient coefficient
        goal_mat.ka = Vec3(params.x + pertubation);
        high_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        goal_mat.ka = Vec3(params.x - pertubation);
        low_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad.x = (high_loss - low_loss) / pertubation2;

        // - Perturb diffuse coefficient
        goal_mat.kd = Vec3(params.y + pertubation);
        high_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        goal_mat.kd = Vec3(params.y - pertubation);
        low_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad.y = (high_loss - low_loss) / pertubation2;

        // - Perturb specular coefficient
        goal_mat.ks = Vec3(params.z + pertubation);
        high_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        goal_mat.ks = Vec3(params.z - pertubation);
        low_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad.z = (high_loss - low_loss) / pertubation2;

        // 2. ADAM Iterates
        m = beta_1 * m + (1.0 - beta_1) * grad;           // Update biased first moment estimate
        v = beta_2 * v + (1.0 - beta_2) * grad.dot(grad); // Update biased second raw moment estimate

        Vec3 m_hat = m / (1.0 - pow(beta_1, iter));       // Compute bias-corrected first moment estimate
        Vec3 v_hat = v / (1.0 - pow(beta_2, iter));       // Compute bias-corrected second raw moment estimate

        params = params - alpha * m_hat / (v_hat.sqrt() + Vec3(epsilon));
        goal_mat.ka = Vec3(params.x);
        goal_mat.kd = Vec3(params.y);
        goal_mat.ks = Vec3(params.z);

        // 3. Diagnostic Information and Early Stopping Conditions
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

            // Early Stopping Conditions
            if (grad_norm < 1e-4 || loss_curr < 1e-7) {
                printf("Early stopping at iteration %d\n", iter);
                break;
            }
        }
    }

    // 4. Print Final Result, Output Final Image
    std::vector<Vec3> final_estimate = scene.render(WIDTH, HEIGHT);
    save_png_from_vec(final_estimate, WIDTH, HEIGHT, "final.png");
    printf("Final params: (%.2f, %.2f, %.2f)\n", params.x, params.y, params.z);
    printf("Final estiamte saved to final.png\n");
}