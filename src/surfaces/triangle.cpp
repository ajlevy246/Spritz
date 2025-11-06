#include <cmath>

#include "surfaces.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

// ===== CONSTRUCTORS =====
Triangle::Triangle() {
    v1 = Vec3(1, 0, 0);
    v2 = Vec3(0, 1, 0);
    v3 = Vec3(0, 0, 1);
    material = Material();
}
Triangle::Triangle(Vec3 vert1, Vec3 vert2, Vec3 vert3, Material mat) {
    v1 = vert1;
    v2 = vert2;
    v3 = vert3;
    material = mat;
}

// ===== METHODS =====
Intersection Triangle::hit(Ray ray, double t0, double t1) {
    // An intersection of a ray with the triangle occurs at time t if:

    // `ray_origin` + `t`*`ray_direction` = `v1` + `\beta(v2 - v1)` + `gamma(v3 - 1)`

    // This is a linear system solvable with Cramer's rule.
    
    // Args:
    //     ray (Ray): Ray to check
    //     t0 (float): Start of time interval
    //     t1 (float): End of time interval

    // Returns: Intersection object
    Intersection hit = Intersection();

    // Compute edges
    Vec3 v2v1 = v1 - v2;
    Vec3 v3v1 = v1 - v3;
    Vec3 dist = v1 - ray.o;

    // Components
    double a = v2v1.x, b = v2v1.y, c = v2v1.z;
    double d = v3v1.x, e = v3v1.y, f = v3v1.z;
    double g = ray.d.x, h = ray.d.y, i = ray.d.z;
    double j = dist.x, k = dist.y, l = dist.z;

    // Determinants
    double ei_hf = e*i - h*f;
    double gf_di = g*f - d*i;
    double dh_eg = d*h - e*g;
    double det = a*ei_hf + b*gf_di + c*dh_eg;
    if (std::fabs(det) < EPS) return hit;

    double ak_jb = a*k - j*b;
    double jc_al = j*c - a*l;
    double bl_kc = b*l - k*c;

    double t = -(f*ak_jb + e*jc_al + d*bl_kc) / det;
    if (t < t0 || t > t1) return hit;

    double gamma = (i*ak_jb + h*jc_al + g*bl_kc) / det;
    if (gamma < 0.0 || gamma > 1.0) return hit;

    double beta = (j*ei_hf + k*gf_di + l*dh_eg) / det;
    if (beta < 0.0 || beta > 1.0 - gamma) return hit;

    // Compute intersection normal
    Vec3 normal = v2v1.cross(v3v1);
    normal.normalize();

    // Fill hit record
    hit.t = t;
    hit.normal = normal;
    hit.surface = this;

    return hit;
}

