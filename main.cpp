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
    
    printf("writing to buffer\n");
    // Create buffer for RGB values (3 channels)
    std::vector<unsigned char> buffer(width * height * 3);
    
    // Convert floating-point colors to bytes
    for (int i = 0; i < width * height; ++i) {
        Vec3 color = pixels[i];
        buffer[i * 3] = static_cast<unsigned char>(std::clamp(color.x, 0.0, 1.0) * 255);
        buffer[i * 3 + 1] = static_cast<unsigned char>(std::clamp(color.y, 0.0, 1.0) * 255);
        buffer[i * 3 + 2] = static_cast<unsigned char>(std::clamp(color.z, 0.0, 1.0) * 255);
    }
    
    printf("writing to .png\n");
    // Write PNG file (3 channels, RGB)
    int success = stbi_write_png(filename.c_str(), width, height, 3, buffer.data(), width * 3);
    
    if (!success) {
        throw std::runtime_error("Failed to write PNG file: " + filename);
    }
    
    std::cout << ".png image written to " << filename << std::endl;
}

// ===== Entry Point =====
int main() {
try {
    // --- Helper materials ---
    Material red_matte = Material(
        Vec3(0.1, 0.0, 0.0),
        Vec3(0.8, 0.1, 0.1),
        Vec3(0.0, 0.0, 0.0),
        0
    );
    Material gold_metal = Material(
        Vec3(0.05, 0.04, 0.0),
        Vec3(0.83, 0.68, 0.21),
        Vec3(0.9, 0.8, 0.4),
        128
    );
    Material blue_mirror = Material(
        Vec3(0.0, 0.0, 0.1),
        Vec3(0.0, 0.0, 0.2),
        Vec3(1, 1, 1),
        256
    );
    Material shiny_plane = Material(
        Vec3(0.3, 0.3, 0.3),
        Vec3(1, 1, 1),
        Vec3(1, 1, 1),
        64
    );

    // --- Build cornell box ---
    Scene scene;
    scene.background = Vec3(0.0, 0.0, 0.0);

    // --- Create camera ---
    Vec3 eye(0, 25, 2);
    Vec3 lookat(0, -1, -0.3);
    double aspect = 1;
    double fov = 114.0;
    scene.set_camera(std::make_shared<PerspectiveCam>(eye, lookat, aspect, fov));

    // --- Walls ---
    Plane left = Plane( // Red
        Vec3(2, 0, 0),
        Vec3(-1, 0, 0),
        Material(
            Vec3(0, 0, 0),
            Vec3(0.75, 0.1, 0.1),
            Vec3(0, 0, 0),
            0
        )
    );
    Plane right = Plane( // Blue
        Vec3(-2, 0, 0),
        Vec3(1, 0, 0),
        Material(
            Vec3(0, 0, 0),
            Vec3(0.1, 0.1, 0.75),
            Vec3(0, 0, 0),
            0
        )
    );
    Plane back = Plane( // White
        Vec3(0, -1, 0),
        Vec3(0, 1, 0),
        Material(
            Vec3(0, 0, 0),
            Vec3(0.5, 0.5, 0.5),
            Vec3(0, 0, 0),
            0
        )
    );
    Plane floor = Plane( // White
        Vec3(0, 0, 0),
        Vec3(0, 0, 1),
        Material(
            Vec3(0, 0, 0),
            Vec3(0.5, 0.5, 0.5),
            Vec3(0, 0, 0),
            0
        )
    );
    Plane ceiling = Plane( // White
        Vec3(0, 0, 5),
        Vec3(0, 0, -1),
        Material(
            Vec3(0, 0, 0),
            Vec3(0.5, 0.5, 0.5),
            Vec3(0, 0, 0),
            0
        )
    );

    // --- Inner Objects ---
    Sphere sphere = Sphere( // Gold Metal
        0.5,
        Vec3(0, 2, -0.5),
        gold_metal
    );

    // --- Light Sources ---
    PointLight light = PointLight(
        Vec3(0, 3, 4),
        Vec3(20, 20, 20)
    );

    // --- Add Objects ---
    scene.add_surface(&left);
    scene.add_surface(&right);
    scene.add_surface(&back);
    scene.add_surface(&floor);
    scene.add_surface(&ceiling);

    scene.add_surface(&sphere);

    // --- Add lights ---
    scene.add_light(&light);

    // --- Render ---
    int width = 1000;
    int height = 1000;
    auto render_start = high_resolution_clock::now();
    std::cout << "Rendering " << width << "x" << height << "..." << std::endl;
    auto pixels = scene.render(width, height);
    auto render_end = high_resolution_clock::now();
    auto render_timing = duration_cast<milliseconds>(render_end - render_start);
    std::cout << "Rendered " << width << "x" << height << " in " << render_timing.count() << " milliseconds" << std::endl;

    // --- Save ppm ---
    // auto ppm_start = high_resolution_clock::now();
    // save_ppm(pixels, width, height, "output.ppm");
    // auto ppm_end = high_resolution_clock::now();
    // auto ppm_timing = duration_cast<milliseconds>(ppm_end - ppm_start);
    // std::cout << "Saved .ppm in " << ppm_timing.count() << " milliseconds" << std::endl;

    // --- Save png
    auto png_start = high_resolution_clock::now();
    save_png(pixels, width, height, "output.png");
    auto png_end = high_resolution_clock::now();
    auto png_timing = duration_cast<milliseconds>(png_end - png_start);
    std::cout << "Saved .png in " << png_timing.count() << " milliseconds" << std::endl;

} catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
}

return 0;

}
