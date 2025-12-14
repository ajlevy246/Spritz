#pragma once

#include "light.h"

class PointLight : public Light {
public:
    // ----- MEMBERS -----
    Vec3 position;
    Vec3 color;
    double intensity;

    // ----- CONSTRUCTORS -----
    PointLight() {
        position = Vec3(0, 0, 0);
        color = Vec3(.5, .5, .5);
        intensity = 10;
    }
    PointLight(Vec3 position_, Vec3 color_, double intensity_) {
        position = position_;
        color = color_;
        intensity = intensity_;
    }

    // ----- METHODS -----
    Vec3 direction_from(const Vec3& point) {
        return (position - point).normalized();
    }

    double distance_from(const Vec3& point) {
        return (position - point).length();
    }

    Vec3 intensity_at(const Vec3& point) { // inverse square law
        double r2 = (position - point).length_sq();
        return color * (intensity / r2);
    }
};