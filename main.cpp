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
    Material chrome(
        Vec3(0.25, 0.25, 0.25),    // ka - ambient (dark gray)
        Vec3(0.4, 0.4, 0.4),       // kd - diffuse (gray, metals have low diffuse)
        Vec3(0.774597, 0.774597, 0.774597),  // ks - specular (bright, highly reflective)
        76,                        // shininess - very sharp highlights
        1.0,                       // ior - not used for opaque metals
        0.0,                       // filter - opaque
        0.8,                       // reflection - highly reflective
        Vec3(1.0, 1.0, 1.0)        // filter_color - not used
    );
    Material gold(
        Vec3(0.24725, 0.1995, 0.0745),     // ka - warm ambient
        Vec3(0.75164, 0.60648, 0.22648),   // kd - golden diffuse
        Vec3(0.628281, 0.555802, 0.366065),// ks - warm specular
        51,                                // shininess - sharp but slightly softer than chrome
        1.0,                               // ior
        0.0,                               // filter - opaque
        0.6,                               // reflection - fairly reflective
        Vec3(1.0, 1.0, 1.0)                // filter_color
    );
    Material frosted_glass(
        Vec3(0.0, 0.0, 0.0),       // ka
        Vec3(0.15, 0.15, 0.15),    // kd - higher diffuse (scattering)
        Vec3(0.7, 0.7, 0.7),       // ks - softer specular
        64,                         // shininess - less sharp
        1.52,                       // ior
        0.70,                       // filter - translucent
        0.25,                       // reflection - visible surface
        Vec3(0.95, 0.95, 0.95)     // filter_color - mostly clear
    );
    Material red_plastic(
        Vec3(0.1, 0.0, 0.0),       // ka - dark red ambient
        Vec3(0.7, 0.1, 0.1),       // kd - bright red diffuse (main color)
        Vec3(0.5, 0.5, 0.5),       // ks - white/gray specular (plastics have colored diffuse, achromatic specular)
        32,                        // shininess - moderate sharpness
        1.46,                      // ior - typical plastic
        0.0,                       // filter - opaque
        0.05,                      // reflection - slight glossy reflection
        Vec3(1.0, 1.0, 1.0)        // filter_color
    );  
    Material matte_clay(
        Vec3(0.2, 0.15, 0.1),      // ka - warm earthy ambient
        Vec3(0.7, 0.5, 0.3),       // kd - terracotta diffuse (dominant component)
        Vec3(0.05, 0.05, 0.05),    // ks - very low specular (matte finish)
        2,                         // shininess - very diffuse, almost no highlight
        1.0,                       // ior
        0.0,                       // filter - opaque
        0.0,                       // reflection - no reflection
        Vec3(1.0, 1.0, 1.0)        // filter_color
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

    // --- Scene ---
    Scene scene = Scene(
        std::make_shared<PerspectiveCam>(Vec3(3, 3, 10),Vec3(0.1, 0.1, 0.1),1.0,114)
    );
    scene.load_background("backgrounds/treetop.hdr");

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

    PointLight sun = PointLight(
        Vec3(-4, -4, 10),
        Vec3(1),
        75
    );
    scene.add_light(&sun);

    // pixels = scene.render(width, height);
    // save_png(pixels, width, height, "output.png");



    // --- Render ---
    int width = 5000;
    int height = 5000;
    std::cout << "Rendering " << width << "x" << height << "..." << std::endl;
    std::vector<Vec3> pixels;
    auto total_start = high_resolution_clock::now();
    int nframes = 240;
    for (int i = 0; i < nframes; i++) {
        auto render_start = high_resolution_clock::now();
        std::cout << "Frame: " << i << std::endl;
        sun.position = Vec3(
            5 * cos(i * 360.0 / nframes * M_PI / 180.0),
            5 * sin(i * 360.0 / nframes * M_PI / 180.0),
            5
        );
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
    // auto png_start = high_resolution_clock::now();
    // std::cout << "Rendering... ";
    // auto pixels = scene.render(width, height);
    // std::cout << " done!" << std::endl;
    // std::cout << "Writing to .png" << std::endl;
    // save_png(pixels, width, height, "output.png");
    // auto png_end = high_resolution_clock::now();
    // auto png_timing = duration_cast<milliseconds>(png_end - png_start);
    // std::cout << "Rendered & Saved .png in " << png_timing.count() << " milliseconds" << std::endl;

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