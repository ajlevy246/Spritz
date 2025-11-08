#include "raytracing.h"

// ===== CONSTRUCTORS =====
Vec3::Vec3() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
}
Vec3::Vec3(double c) {
    x = c;
    y = c;
    z = c;
}
Vec3::Vec3(double new_x, double new_y, double new_z) {
    x = new_x;
    y = new_y;
    z = new_z;
}

// ===== OPERATORS =====
Vec3 Vec3::operator+(const Vec3& v) {
    return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator-(const Vec3& v) {
    return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator*(double c) {
    return Vec3(c*x, c*y, c*z);
}

Vec3 Vec3::operator*(const Vec3& v) {
    return Vec3(x * v.x, y * v.y, z * v.z);
}

Vec3 Vec3::operator/(double c) {
    return Vec3(x/c, y/c, z/c);
}

Vec3 Vec3::operator/(const Vec3& v) {
    return Vec3(x / v.x, y / v.y, z/v.z);
}

// ===== METHODS =====
double Vec3::dot(const Vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

Vec3 Vec3::cross(const Vec3& v) const {
    double cross_x = y*v.z - z*v.y;
    double cross_y = z*v.x - x*v.z;
    double cross_z = x*v.y - y*v.x;
    return Vec3(cross_x, cross_y, cross_z); 
}

double Vec3::length() const {
    return sqrt(x*x + y*y + z*z);
}

double Vec3::length_sq() const {
    return (x*x + y*y + z*z);
}

void Vec3::normalize() {
    double len = length();
    if (len > 0.0) {
        x = x / len;
        y = y / len;
        z = z / len;
    }
}

Vec3 Vec3::normalized() const {
    double len = length();
    if (len > 0.0) {
        return Vec3(x / len, y / len, z / len);
    }
    return Vec3(0, 0, 0);
}