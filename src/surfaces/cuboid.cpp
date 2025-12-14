#include <algorithm>

#include "surfaces/cuboid.h"
#include "materials.h"

// ===== CONSTRUCTORS =====
Cuboid::Cuboid(Vec3 min_, Vec3 max_ , Material* mat) {
    min_p = min_;
    max_p = max_;
    material = mat;
}

// ===== METHODS =====
Intersection Cuboid::hit(Ray ray, double t0_, double t1_) {
    Intersection hit = Intersection();

    double tmin = -INFINITY, tmax = INFINITY;

    auto check_axis = [&](double origin, double direction, double minB, double maxB) {
        if (fabs(direction) < EPS) {
            if (origin < minB || origin > maxB) return false;
            return true;
        }
        double t1 = (minB - origin) / direction;
        double t2 = (maxB - origin) / direction;
        if (t1 > t2) std::swap(t1, t2);
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        return tmin <= tmax;
    };

    if (!check_axis(ray.o.x, ray.d.x, min_p.x, max_p.x)) return hit;
    if (!check_axis(ray.o.y, ray.d.y, min_p.y, max_p.y)) return hit;
    if (!check_axis(ray.o.z, ray.d.z, min_p.z, max_p.z)) return hit;

    if (tmax < 0 || tmin > tmax) return hit;

    double t_hit = (tmin >= t0_) ? tmin : tmax;
    if (t_hit < t0_ || t_hit > t1_) return hit;

    hit.t = t_hit;
    hit.surface = this;

    // Determine normal
    Vec3 p = ray.evaluate(hit.t);
    if (fabs(p.x - min_p.x) < EPS) hit.normal = Vec3(-1, 0, 0);
    else if (fabs(p.x - max_p.x) < EPS) hit.normal = Vec3(1, 0, 0);
    else if (fabs(p.y - min_p.y) < EPS) hit.normal = Vec3(0, -1, 0);
    else if (fabs(p.y - max_p.y) < EPS) hit.normal = Vec3(0, 1, 0);
    else if (fabs(p.z - min_p.z) < EPS) hit.normal = Vec3(0, 0, -1);
    else if (fabs(p.z - max_p.z) < EPS) hit.normal = Vec3(0, 0, 1);

    return hit;
}
