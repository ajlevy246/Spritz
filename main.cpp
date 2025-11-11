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


// Save function from your snippet
void save_ppm(const std::vector<Vec3>& pixels, int width, int height, const std::string& filename) {
std::ofstream out(filename, std::ios::out | std::ios::binary);
if (!out) throw std::runtime_error("Could not open file for writing: " + filename);

std::cout << "finished render" << std::endl;
out << "P3\n" << width << " " << height << "\n255\n";
for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
        int idx = y * width + x;
        Vec3 color = pixels[idx];
        double r = std::clamp(color.x, 0.0, 1.0);
        double g = std::clamp(color.y, 0.0, 1.0);
        double b = std::clamp(color.z, 0.0, 1.0);
        int ir = static_cast<int>(255.99 * r);
        int ig = static_cast<int>(255.99 * g);
        int ib = static_cast<int>(255.99 * b);
        out << ir << " " << ig << " " << ib << "\n";
    }
}
std::cout << ".ppm image written to " << filename << std::endl;


}

void save_png(const std::vector<Vec3>& pixels, int width, int height, const std::string& filename) {
    
    // printf("writing to buffer\n");
    // Create buffer for RGB values (3 channels)
    std::vector<unsigned char> buffer(width * height * 3);
    
    // Convert floating-point colors to bytes
    for (int i = 0; i < width * height; ++i) {
        Vec3 color = pixels[i];
        buffer[i * 3] = static_cast<unsigned char>(std::clamp(color.x, 0.0, 1.0) * 255);
        buffer[i * 3 + 1] = static_cast<unsigned char>(std::clamp(color.y, 0.0, 1.0) * 255);
        buffer[i * 3 + 2] = static_cast<unsigned char>(std::clamp(color.z, 0.0, 1.0) * 255);
    }
    
    // printf("writing to .png\n");
    // Write PNG file (3 channels, RGB)
    int success = stbi_write_png(filename.c_str(), width, height, 3, buffer.data(), width * 3);
    
    if (!success) {
        throw std::runtime_error("Failed to write PNG file: " + filename);
    }
    
    // std::cout << ".png image written to " << filename << std::endl;
}

// ===== Entry Point =====
int main() {
try {
    // --- Helper materials ---

    // ===== METALS =====
    Material chrome(
        Vec3(0.25, 0.25, 0.25),  // ka - ambient
        Vec3(0.4, 0.4, 0.4),     // kd - diffuse
        Vec3(0.774597, 0.774597, 0.774597), // ks - specular
        76,                       // shininess
        1.0,                      // ior (not used for opaque)
        0.0,                      // filter (opaque)
        0.3,                      // reflection (highly reflective)
        Vec3(1, 1, 1)            // filter_color (not used)
    );

    Material gold(
        Vec3(0.24725, 0.1995, 0.0745),
        Vec3(0.75164, 0.60648, 0.22648),
        Vec3(0.628281, 0.555802, 0.366065),
        51,
        1.0,
        0.0,
        0.7,
        Vec3(1, 1, 1)
    );

    Material copper(
        Vec3(0.19125, 0.0735, 0.0225),
        Vec3(0.7038, 0.27048, 0.0828),
        Vec3(0.256777, 0.137622, 0.086014),
        12,
        1.0,
        0.0,
        0.6,
        Vec3(1, 1, 1)
    );

    // ===== GLASS / TRANSPARENT =====
    Material clear_glass(
        Vec3(0.0, 0.0, 0.0),     // ka - no ambient for transparent
        Vec3(0.1, 0.1, 0.1),     // kd - minimal diffuse
        Vec3(0.9, 0.9, 0.9),     // ks - high specular
        100,                      // shininess - very smooth
        1.52,                     // ior - typical glass
        0.95,                     // filter - highly transparent
        0.05,                     // reflection - low direct reflection (Fresnel dominates)
        Vec3(1.0, 1.0, 1.0)      // filter_color - clear
    );

    Material red_glass(
        Vec3(0.0, 0.0, 0.0),
        Vec3(0.1, 0.0, 0.0),
        Vec3(0.9, 0.9, 0.9),
        100,
        1.52,
        0.9,
        0.05,
        Vec3(1.0, 0.1, 0.1)      // filter_color - red tinted
    );

    Material green_glass(
        Vec3(0.0, 0.0, 0.0),
        Vec3(0.0, 0.1, 0.0),
        Vec3(0.9, 0.9, 0.9),
        100,
        1.52,
        0.9,
        0.05,
        Vec3(0.1, 1.0, 0.1)      // filter_color - green tinted
    );

    Material amber_glass(
        Vec3(0.0, 0.0, 0.0),
        Vec3(0.1, 0.07, 0.0),
        Vec3(0.9, 0.9, 0.9),
        100,
        1.52,
        0.85,
        0.05,
        Vec3(1.0, 0.7, 0.2)      // filter_color - amber/honey color
    );

    // ===== LIQUIDS =====
    Material water(
        Vec3(0.0, 0.0, 0.0),
        Vec3(0.05, 0.05, 0.05),
        Vec3(0.8, 0.8, 0.8),
        100,
        1.33,                     // ior - water
        0.98,                     // filter - very transparent
        0.02,
        Vec3(0.9, 0.95, 1.0)     // filter_color - slight blue tint
    );

    Material diamond(
        Vec3(0.0, 0.0, 0.0),
        Vec3(0.1, 0.1, 0.1),
        Vec3(1.0, 1.0, 1.0),
        150,                      // very high shininess
        2.42,                     // ior - diamond (high refraction!)
        0.9,
        0.1,
        Vec3(1.0, 1.0, 1.0)
    );

    // ===== DIFFUSE / MATTE =====
    Material matte_white(
        Vec3(0.1, 0.1, 0.1),
        Vec3(0.5, 0.5, 0.5),
        Vec3(0.1, 0.1, 0.1),     // low specular
        4,                       // low shininess
        1.0,
        0.0,
        0.0,                      // no reflection
        Vec3(1, 1, 1)
    );

    Material matte_red(
        Vec3(0.1, 0.0, 0.0),
        Vec3(0.8, 0.1, 0.1),
        Vec3(0.2, 0.2, 0.2),
        10,
        1.0,
        0.0,
        0.0,
        Vec3(1, 1, 1)
    );

    // ===== PLASTIC =====
    Material plastic_shiny(
        Vec3(0.05, 0.05, 0.35),
        Vec3(0.6, 0.6, 0.8),
        Vec3(0.7, 0.7, 0.7),
        32,                       // medium shininess
        1.0,
        0.0,
        0.3,                      // some reflection
        Vec3(1, 1, 1)
    );

    // ===== MIRROR =====
    Material mirror(
        Vec3(0.1, 0.1, 0.1),
        Vec3(0.1, 0.1, 0.1),     // very low diffuse
        Vec3(1.0, 1.0, 1.0),
        200,
        1.0,
        0.0,
        0.95,                     // almost pure reflection
        Vec3(1, 1, 1)
    );


    // ===== SCENE ===== 
    Scene scene = Scene();
    std::cout << "Loading scene... ";
    scene.load_background("backgrounds/clear_night4.hdr");
    scene.max_bounces = 2;

    scene.set_camera(std::make_shared<PerspectiveCam>(
        Vec3(6, 6, 2),
        Vec3(6, -1, 0),
        1.0,
        114.0
    ));
 
    // ===== LIGHTS =====
    PointLight point_a = PointLight(
        Vec3(2.5, 0, 5),
        Vec3(0, 1, 0),
        25
    );
    scene.add_light(&point_a);
    PointLight point_b = PointLight(
        Vec3(0, 2.5, 5),
        Vec3(0, 0, 1),
        25
    );
    scene.add_light(&point_b);
    PointLight point_c = PointLight(
        Vec3(-1.414, -1.414, 5),
        Vec3(1, 0, 0),
        25
    );
    scene.add_light(&point_c);

    // ===== OBJECTS =====
    Triangle test = Triangle(
        Vec3(3, 3, 0),
        Vec3(6, 2, 3),
        Vec3(9, 3, 0),
        &clear_glass
    );
    // scene.add_surface(&test);
    Sphere sphere_a = Sphere(
        1,
        Vec3(3, 0, 0),
        &clear_glass
    );
    scene.add_surface(&sphere_a);
    Sphere sphere_b = Sphere(
        1, 
        Vec3(0, 3, 0),
        &clear_glass
    );
    scene.add_surface(&sphere_b);
    Sphere sphere_c = Sphere(
        1,
        Vec3(-1.414, -1.414, 0),
        &clear_glass
    );
    scene.add_surface(&sphere_c);
    Sphere sphere_d = Sphere(
        1,
        Vec3(9, 0, 0),
        &clear_glass
    );
    // scene.add_surface(&sphere_d);
    Sphere sphere_e = Sphere(
        1, 
        Vec3(12, 0, 0),
        &amber_glass
    );
    // scene.add_surface(&sphere_e);
    Plane plane = Plane(
        Vec3(0, 0, 1),
        Vec3(0, 0, -1),
        &matte_white
    );
    scene.add_surface(&plane);

    Triangle tri_a = Triangle(
        Vec3(0, 0, 0),
        Vec3(1, 1, 1),
        Vec3(0, 2, 0),
        &clear_glass
    );
    Triangle tri_b = Triangle(
        Vec3(0, 2, 0),
        Vec3(1, 1, 1),
        Vec3(2, 2, 0),
        &clear_glass
    );
    Triangle tri_c = Triangle(
        Vec3(2, 2, 0),
        Vec3(1, 1, 1),
        Vec3(2, 0, 0),
        &clear_glass
    );
    Triangle tri_d = Triangle(
        Vec3(2, 0, 0),
        Vec3(1, 1, 1),
        Vec3(0, 0, 0),
        &clear_glass
    );
    // scene.add_surface(&tri_a);
    // scene.add_surface(&tri_b);
    // scene.add_surface(&tri_c);
    // scene.add_surface(&tri_d);
    std::cout << " done!" << std::endl;

    // --- Render ---
    int width = 2000;
    int height = 2000;
    std::cout << "Rendering " << width << "x" << height << "..." << std::endl;
    std::vector<Vec3> pixels;
    auto total_start = high_resolution_clock::now();
    int nframes = 240;
    for (int i = 0; i < nframes; i++) {
        auto render_start = high_resolution_clock::now();
        std::cout << "Frame: " << i << std::endl;
        scene.set_camera(std::make_shared<PerspectiveCam>(
            Vec3(6 * cos(i * 360 / nframes * M_PI / 180), 6 * sin(i * 360 / nframes * M_PI / 180), 0.5),
            Vec3(0, 0, 0),
            1.0,
            114
        ));
        pixels = scene.render(width, height);
        auto render_end = high_resolution_clock::now();
        auto render_timing = duration_cast<milliseconds>(render_end - render_start);
        save_png(pixels, width, height, "animation/frame" + std::to_string(i) + ".png");
        // std::cout << "Rendered " << width << "x" << height << " in " << render_timing.count() << " milliseconds" << std::endl;
    }
    auto total_end = high_resolution_clock::now();
    std::cout << "Animated in " << duration_cast<seconds>(total_end - total_start).count() << " seconds." << std::endl;

    // --- Save ppm ---
    // auto ppm_start = high_resolution_clock::now();
    // save_ppm(pixels, width, height, "output.ppm");
    // auto ppm_end = high_resolution_clock::now();
    // auto ppm_timing = duration_cast<milliseconds>(ppm_end - ppm_start);
    // std::cout << "Saved .ppm in " << ppm_timing.count() << " milliseconds" << std::endl;

    // --- Save png
    auto png_start = high_resolution_clock::now();
    std::cout << "Rendering... ";
    pixels = scene.render(width, height);
    std::cout << " done!" << std::endl;
    std::cout << "Writing to .png" << std::endl;
    save_png(pixels, width, height, "output.png");
    auto png_end = high_resolution_clock::now();
    auto png_timing = duration_cast<milliseconds>(png_end - png_start);
    std::cout << "Rendered & Saved .png in " << png_timing.count() << " milliseconds" << std::endl;

} catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
}

return 0;

}

// Measures of optimizations, 5000x5000 image, three balls three lights
// OPTIMIZATION               TOTAL TIME (seconds)
// ====================================================== 
// PRE PARALLEL:              14.319
// AUTO PARALLEL:             3.175
// Vec3 Optimizations:        2.57