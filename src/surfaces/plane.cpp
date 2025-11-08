#include <cmath>

#include "surfaces.h"

// ===== CONSTRUCTORS =====
Plane::Plane(Vec3 plane_normal, Vec3 plane_point, Material* mat) {
    n = plane_normal;
    p = plane_point;
    material = mat;
}

// ===== METHODS =====
Intersection Plane::hit(Ray ray, double t0, double t1) {
    // An intersection of a ray and a plane occurs at time t when:
    
    // `n * (ray_origin + t*ray_direction - x) = 0`, where x is a point on the plane

    // Args:
    //     ray (tuple[np.array]): _description_
    //     t0 (int, optional): _description_. Defaults to 0.
    //     t1 (_type_, optional): _description_. Defaults to np.inf.
    Intersection hit = Intersection();

    double denom = n.dot(ray.d);
    if (std::fabs(denom) < EPS) {return hit;}
    Vec3 dist = p - ray.o;
    double t = n.dot(dist) / denom;
    if (t0 <= t && t <= t1) {
        hit.t = t;
        hit.normal = n;
        hit.surface = this;
    }
    return hit;
}