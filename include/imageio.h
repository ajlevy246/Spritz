// image_io.h
#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "raytracing.h"

// Loads a PNG image into a vector of Vec3 (RGB in [0,1])
inline std::vector<Vec3> load_png_as_vec(
    const std::string& filename,
    int& width,
    int& height
) {
    int channels;

    // Force RGB output
    unsigned char* data = stbi_load(
        filename.c_str(),
        &width,
        &height,
        &channels,
        3
    );

    if (!data) {
        throw std::runtime_error(
            "Failed to load image: " + filename
        );
    }

    std::vector<Vec3> image;
    image.reserve(width * height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = 3 * (y * width + x);

            float r = data[idx + 0] / 255.0f;
            float g = data[idx + 1] / 255.0f;
            float b = data[idx + 2] / 255.0f;

            image.emplace_back(r, g, b);
        }
    }

    stbi_image_free(data);
    return image;
}


// Save a render to disk
void save_png_from_vec(const std::vector<Vec3>& pixels, int width, int height, const std::string& filename) {

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