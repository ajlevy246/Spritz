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
#include "surfaces.h"

using namespace std::chrono;

// UTILITY FUNCTIONS

// Save a render to disk
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

// Render simple glass cube from reference.
int main() {
    std::shared_ptr cam = std::make_shared<PerspectiveCam> (
        Vec3(2, 2, 2),
        Vec3(0, 0, 0),
        1.0,
        100.0
    );

    Scene scene(cam);
    scene.load_background("backgrounds/cobble.hdr");
    
    TriangularMesh mesh("assets/pinguinolow.obj", &red_plastic);
    scene.add_surface(&mesh);

    PointLight light_a(
        Vec3(5, 5, 5),
        Vec3(1),
        50
    );
    scene.add_light(&light_a);

    std::vector<Vec3> image;
    for (int i = 0; i < 360; i++) {
        cam = std::make_shared<PerspectiveCam>(
            Vec3(50 * cos(i * M_PI / 360.0f), 50 * sin(i * M_PI / 360.0f), 4),
            Vec3(0),
            1.0,
            114.0
        );
        scene.set_camera(cam);
        image = scene.render(1000, 1000);
        save_png(image, 1000, 1000, "obj_renders/obj" + std::to_string(i) + ".png");
    }
    
    return 0;
}