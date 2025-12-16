// axis-aligned bounding boxes for acceleration structures
#pragma once

#include "raytracing/vec3.h"
#include "raytracing/ray.h"
#include <cmath>
#include <iostream>

class AABB {
public:
    // ===== MEMBERS =====
    Vec3 minv;
    Vec3 maxv;

    // ===== CONSTRUCTORS =====
    AABB(Vec3 min_v, Vec3 max_v) {
        minv = min_v;
        maxv = max_v;
    }

    AABB() {
        minv = Vec3(MAXFLOAT);
        maxv = Vec3(-MAXFLOAT);
    }

    // ===== METHODS =====
    
    // test intersection - Smit's method
    // bool intersect (const Ray& ray, double t0, double t1) const {
    //     double divx, tmin, tmax, tymin, tymax, tzmin, tzmax;
    //     // test x-dir
    //     divx = 1 / ray.d.x;
    //     if (divx >= 0) {
    //         tmin = (minv.x - ray.o.x) * divx;
    //         tmax = (maxv.x - ray.o.x) * divx;
    //     } else {
    //         tmin = (maxv.x - ray.o.x) * divx;
    //         tmax = (minv.x - ray.o.x) * divx; 
    //     }

    //     // test y-dir
    //     if (ray.d.y >= 0) {
    //         tymin = (minv.y - ray.o.y) / ray.d.y;
    //         tymax = (maxv.y - ray.o.y) / ray.d.y;
    //     } else {
    //         tymin = (maxv.y - ray.o.y) / ray.d.y;
    //         tymax = (minv.y - ray.o.y) / ray.d.y;
    //     }

    //     // early stopping tests
    //     if ( (tmin > tymax) || (tymin > tmax) ) return false;
    //     if (tymin > tmin) tmin = tymin;
    //     if (tymax < tmax) tmax = tymax;

    //     // test z-dir
    //     if (ray.d.z >= 0) {
    //         tzmin = (minv.z - ray.o.z) / ray.d.z;
    //         tzmax = (maxv.z - ray.o.z) / ray.d.z;
    //     } else {
    //         tzmin = (maxv.z - ray.o.z) / ray.d.z;
    //         tzmax = (minv.z - ray.o.z) / ray.d.z;
    //     }

    //     if ( (tmin > tzmax) || (tzmin > tmax) ) return false;
    //     if (tzmin > tmin) tmin = tzmin;
    //     if (tzmax < tmax) tmax = tzmax;
    //     return ( (tmin < t1) && (tmax > t0) );
    // }
    bool intersect(const Ray& ray, double t0, double t1) const {
        double tmin = t0;
        double tmax = t1;

        auto slab = [&](double o, double d, double minv, double maxv) {
            if (std::abs(d) < 1e-8) {
                // Ray parallel to slab
                if (o < minv || o > maxv) return false;
                return true;
            }
            double invD = 1.0 / d;
            double t0s = (minv - o) * invD;
            double t1s = (maxv - o) * invD;
            if (t0s > t1s) std::swap(t0s, t1s);
            tmin = std::max(tmin, t0s);
            tmax = std::min(tmax, t1s);
            return tmin <= tmax;
        };

        if (!slab(ray.o.x, ray.d.x, minv.x, maxv.x)) return false;
        if (!slab(ray.o.y, ray.d.y, minv.y, maxv.y)) return false;
        if (!slab(ray.o.z, ray.d.z, minv.z, maxv.z)) return false;

        return true;
    }

};