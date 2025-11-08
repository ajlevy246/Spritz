#pragma once

#include <cmath>

class Vec3 {
public:
    // ===== MEMBERS =====
    double x, y, z;

    // ===== CONSTRUCTORS =====
    Vec3();
    Vec3(double c);
    Vec3(double new_x, double new_y, double new_z);

    // ===== OPERATORS =====
    Vec3 operator+(const Vec3& v);
    Vec3 operator-(const Vec3& v);
    Vec3 operator*(double c);      // Scalar mult
    Vec3 operator*(const Vec3& v); // Component-wise mult
    Vec3 operator/(double c);
    Vec3 operator/(const Vec3& v);

    // ===== METHODS =====
    void normalize();

    double dot(const Vec3& v) const;
    Vec3 cross(const Vec3& v) const;
    double length() const;
    double length_sq() const;
    Vec3 normalized() const;
};