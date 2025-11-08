#include <cmath>
#include <algorithm>

#include "surfaces.h"
#include "raytracing.h"
#include "materials.h"

// ===== CONSTRUCTORS =====
Sphere::Sphere(double radius, Vec3 center, Material* mat) {
    r = radius;
    c = center;
    material = mat;
}

// ===== METHODS =====
Intersection Sphere::hit(Ray ray, double t0, double t1) {
// An intersection of a ray with the sphere occurs at time t if:

//     |(`ray_origin` + `t`*`ray_direction` - `sphere_center`)| - `sphere_radius`^2 = 0

//     This is a quadratic: `Ax^2 + Bx + C`.
    
//     Args:
//         ray (Ray): Ray to check
//         t0 (float): Start of time interval
//         t1 (float): End of time interval

// Returns: intersection object, at t=inf if no hit
    Intersection hit = Intersection();
    
    Vec3 dist = ray.o - c;
    // Solve quadratic
    double A = ray.d.length_sq();
    double B = ray.d.dot(dist);
    double C = dist.length_sq() - pow(r, 2);

    double discriminant = pow(B, 2) - A * C;
    if (discriminant < 0) { //no real solutions
        return hit;
    }
    double dsqrt = sqrt(discriminant);
    double h_1 = (-B + dsqrt) / A;
    double h_2 = (-B - dsqrt) / A;

    double t;
    if (t0 <= h_1 && h_1 <= t1) {
        if (t0 <= h_2 && h_2 <= t1) {
            t = std::min(h_1, h_2);
        } else {
            t = h_1;
        }
    } else {
        if (t0 <= h_2 && h_2 <= t1) {
            t = h_2;
        } else { // Neither intersection point lies in the time interval
            // i.e., another object lies in between this one and ray origin.
            return hit;
        }
    }

    Vec3 intersection_point = ray.evaluate(t);
    Vec3 normal = (intersection_point - c) / r;
    hit.normal = normal;
    hit.t = t;
    hit.surface = this;
    return hit;
}