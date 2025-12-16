#include <cmath>

#include "surfaces.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

// ===== CONSTRUCTORS =====
Triangle::Triangle(Vec3 vert1, Vec3 vert2, Vec3 vert3, Material* mat) {
    v1 = vert1;
    v2 = vert2;
    v3 = vert3;
    edge1 = v2 - v1;
    edge2 = v3 - v1;
    normal = edge1.cross(edge2).normalized();
    material = mat;
}

// ===== METHODS =====

// An intersection of a ray with the triangle occurs at time t if:
// `ray_origin` + `t`*`ray_direction` = `v1` + `\beta(v2 - v1)` + `gamma(v3 - 1)`
// This is a linear system solvable with Cramer's rule.
Intersection Triangle::hit(const Ray& ray, double t0, double t1) {
    
    Intersection hit; // empty hit record denotes a miss

    const double EPS = 1e-8;
    Vec3 pvec = ray.d.cross(edge2);
    double det = edge1.dot(pvec);

    if (fabs(det) < EPS) return hit; // Ray parallel to triangle

    double invDet = 1.0 / det;
    Vec3 tvec = ray.o - v1;
    double u = tvec.dot(pvec) * invDet;
    if (u < 0.0 || u > 1.0) return hit;

    Vec3 qvec = tvec.cross(edge1);
    double v = ray.d.dot(qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return hit;

    double t = edge2.dot(qvec) * invDet;
    if (t < t0 || t > t1) return hit;

    hit.t = t;
    if (normal.dot(ray.d) > 0) {
        hit.normal = normal * -1;
    }
    else {
        hit.normal = normal;
    }
    hit.surface = this;
    return hit;
}
