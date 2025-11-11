#pragma once

#include <cmath>

class Vec3 {
public:
    // ===== MEMBERS =====
    double x, y, z;

    // ===== CONSTRUCTORS =====
    inline Vec3() : x(0.0), y(0.0), z(0.0) {}
    inline Vec3(double c) : x(c), y(c), z(c) {}
    inline Vec3(double new_x, double new_y, double new_z) : x(new_x), y(new_y), z(new_z) {}

    // ===== OPERATORS =====
    inline Vec3 operator+(const Vec3& v) const {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }
    
    inline Vec3 operator-(const Vec3& v) const {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }
    
    inline Vec3 operator*(double c) const {
        return Vec3(x * c, y * c, z * c);
    }
    
    inline Vec3 operator*(const Vec3& v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }
    
    inline Vec3 operator/(double c) const {
        double inv = 1.0 / c;  // Division is slower than multiplication
        return Vec3(x * inv, y * inv, z * inv);
    }
    
    inline Vec3 operator/(const Vec3& v) const {
        return Vec3(x / v.x, y / v.y, z / v.z);
    }

    // ===== METHODS =====
    inline void normalize() {
        double len_sq = x * x + y * y + z * z;
        if (len_sq > 0.0) {
            double inv_len = 1.0 / std::sqrt(len_sq);
            x *= inv_len;
            y *= inv_len;
            z *= inv_len;
        }
    }

    inline double dot(const Vec3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }
    
    inline Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    
    inline double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    inline double length_sq() const {
        return x * x + y * y + z * z;
    }
    
    inline Vec3 normalized() const {
        double len_sq = x * x + y * y + z * z;
        if (len_sq > 0.0) {
            double inv_len = 1.0 / std::sqrt(len_sq);
            return Vec3(x * inv_len, y * inv_len, z * inv_len);
        }
        return Vec3(0.0, 0.0, 0.0);
    }
};

// Also add this for convenience: scalar * vector
inline Vec3 operator*(double c, const Vec3& v) {
    return Vec3(v.x * c, v.y * c, v.z * c);
}