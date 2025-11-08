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
    Material matte(
        Vec3(0.05, 0.05, 0.05),
        Vec3(0.7, 0.4, 0.3),
        Vec3(0.0, 0.0, 0.0),
        8,
        1.0,
        0.0,
        0.0
    );
    Material plastic(
        Vec3(0.05),                  // ka
        Vec3(0.6, 0.1, 0.1),         // kd (reddish)
        Vec3(0.6),                   // ks (white specular tint)
        64,                          // shininess
        1.3,                         // ior (typical for plastics)
        0.0,                         // filter (opaque)
        0.1                          // reflection (slight mirror contribution)
    );
    Material metal(
        Vec3(0.05),
        Vec3(0.2, 0.2, 0.2),         // kd (subtle diffuse base)
        Vec3(0.9, 0.8, 0.7),         // ks (gold-like specular)
        128,
        2.5,                         // ior (metals have high apparent IOR)
        0.0,                         // filter (opaque)
        0.9                          // reflection (strong mirror)
    );
    Material glass(
        Vec3(0.02),
        Vec3(0.1),                   // kd (almost black diffuse)
        Vec3(0.9),                   // ks (white specular)
        128,
        1.52,                        // ior (common for glass)
        1.0,                         // filter (fully transmissive)
        0.05                         // reflection (small base reflection)
    );
    Material water(
        Vec3(0.02),
        Vec3(0.1, 0.15, 0.2),        // kd (bluish)
        Vec3(0.8),                   // ks
        64,
        1.33,                        // ior (water)
        0.9,                         // filter (transmissive)
        0.05                         // reflection (small)
    );
    Material frosted(
        Vec3(0.05),
        Vec3(0.5),                   // kd (light gray diffuse)
        Vec3(0.4),                   // ks
        16,                          // shininess (blurry specular)
        1.45,                        // ior (glass-like)
        0.7,                         // filter (partially transmissive)
        0.2                          // reflection
    );
    Material blue_glass(
        Vec3(0.02),              // ka: tiny ambient
        Vec3(0.05),              // kd: almost no diffuse
        Vec3(0.8),               // ks: bright specular
        128,                     // shiny highlights
        1.52,                    // ior: standard glass
        1.0,                     // filter: fully transparent
        0.05                     // reflection: small
    );
    Material green_crystal(
        Vec3(0.01),
        Vec3(0.02),
        Vec3(0.8),
        256,
        1.8,                     // higher IOR (stronger refraction)
        1.0,
        0.05
    );
    Material red_glass(
        Vec3(0.01),
        Vec3(0.02),
        Vec3(0.8),
        64,
        1.3,
        1.0,
        0.05
    );
    Material diamond(
        Vec3(0.01),
        Vec3(0.05),
        Vec3(0.9),
        256,
        2.42,                    // diamond IOR
        1.0,
        0.3                      // stronger reflection
    );
    Material air(
        Vec3(0.0),
        Vec3(0.0),
        Vec3(0.0),
        1,
        1.0,                     // nearly equal to outside medium
        1.0,
        0.0
    );


    // ===== SCENE ===== 
    Scene scene = Scene();
    std::cout << "Loading scene... ";
    scene.load_background("backgrounds/sunrise.hdr");
    scene.max_bounces = 2;

    scene.set_camera(std::make_shared<PerspectiveCam>(
        Vec3(6, 6, 0),
        Vec3(6, -1, 0.1),
        1.0,
        114.0
    ));
 
    // ===== LIGHTS =====
    PointLight point = PointLight(
        Vec3(2.5, 0, 5),
        Vec3(1, 1, 1),
        50
    );
    scene.add_light(&point);

    // ===== OBJECTS =====
    Sphere test = Sphere(
        1,
        Vec3(6, -3, 0),
        &matte
    );
    scene.add_surface(&test);
    Sphere sphere_a = Sphere(
        1, 
        Vec3(0, 0, 0),
        &blue_glass
    );
    scene.add_surface(&sphere_a);
    Sphere sphere_b = Sphere(
        1, 
        Vec3(3, 0, 0),
        &green_crystal
    );
    scene.add_surface(&sphere_b);
    Sphere sphere_c = Sphere(
        1,
        Vec3(6, 0, 0),
        &diamond
    );
    scene.add_surface(&sphere_c);
    Sphere sphere_d = Sphere(
        1,
        Vec3(9, 0, 0),
        &glass
    );
    scene.add_surface(&sphere_d);
    Sphere sphere_e = Sphere(
        1, 
        Vec3(12, 0, 0),
        &frosted
    );
    scene.add_surface(&sphere_e);
    Plane plane = Plane(
        Vec3(0, 0, 1),
        Vec3(0, 0, -1),
        &water
    );
    scene.add_surface(&plane);
    std::cout << " done!" << std::endl;

    // --- Render ---
    int width = 5000;
    int height = 5000;
    // std::cout << "Rendering " << width << "x" << height << "..." << std::endl;
    // std::vector<Vec3> pixels;
    // auto total_start = high_resolution_clock::now();
    // int nframes = 240;
    // for (int i = 0; i < nframes; i++) {
    //     auto render_start = high_resolution_clock::now();
    //     std::cout << "Frame: " << i << std::endl;
    //     scene.set_camera(std::make_shared<PerspectiveCam>(
    //         Vec3(0, 0, 0),
    //         Vec3(cos(i * 360 / nframes * M_PI / 180), sin(i * 360 / nframes * M_PI / 180), 0),
    //         1.0,
    //         114
    //     ));
    //     pixels = scene.render(width, height);
    //     auto render_end = high_resolution_clock::now();
    //     auto render_timing = duration_cast<milliseconds>(render_end - render_start);
    //     save_png(pixels, width, height, "animation/frame" + std::to_string(i) + ".png");
    //     // std::cout << "Rendered " << width << "x" << height << " in " << render_timing.count() << " milliseconds" << std::endl;
    // }
    // auto total_end = high_resolution_clock::now();
    // std::cout << "Animated in " << duration_cast<seconds>(total_end - total_start).count() << " seconds." << std::endl;

    // --- Save ppm ---
    // auto ppm_start = high_resolution_clock::now();
    // save_ppm(pixels, width, height, "output.ppm");
    // auto ppm_end = high_resolution_clock::now();
    // auto ppm_timing = duration_cast<milliseconds>(ppm_end - ppm_start);
    // std::cout << "Saved .ppm in " << ppm_timing.count() << " milliseconds" << std::endl;

    // --- Save png
    auto png_start = high_resolution_clock::now();
    printf("Rendering...");
    auto pixels = scene.render(width, height);
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
