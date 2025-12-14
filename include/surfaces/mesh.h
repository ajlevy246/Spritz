// mesh.h - Triangular mesh implementation

#pragma once

#include "surface.h"
#include "materials.h"
#include "raytracing.h"

struct TriangularFace { 
    int idx0, idx1, idx2; // indices of the three vertices that make up this face.
};

class TriangularMesh : public Surface {
public:
    // ===== MEMBERS =====
    std::vector<Vec3> vertices; // One Vec3 for each vertex
    std::vector<TriangularFace> faces; // On TriangularFace for each face

    // ===== CONSTRUCTORS =====
    
    // Load mesh from .obj file
    TriangularMesh(const std::string &filename, Material* mat);

    // Load Mesh from vertices and face indices lists.
    TriangularMesh(std::vector<Vec3> verts, std::vector<TriangularFace> faceIndices, Material* mat) {
        vertices = verts;
        faces = faceIndices;
        material = mat;
    }

    // ===== METHODS =====
    
    // set new object material
    void setMaterial(Material *mat) {
        material = mat;   
    }

    // ray-triangle intersection with moller-trumbore
    // - hits return true and populate t with the time of intersection
    // - misses return false
    static bool intersect(
        const Ray& ray, 
        const Vec3& v1, 
        const Vec3& v2, 
        const Vec3& v3,
        double &t
        ) 
    {
        const double EPS = 1e-8;
        Vec3 edge1 = v2 - v1;
        Vec3 edge2 = v3 - v1;

        Vec3 pvec = ray.d.cross(edge2);
        double det = edge1.dot(pvec);

        if (fabs(det) < EPS) return false; // ray is parallel to triangle

        double invDet = 1.0 / det;
        Vec3 tvec = ray.o - v1;
        double u = tvec.dot(pvec) * invDet;
        if (u < 0.0 || u > 1.0) return false;

        Vec3 qvec = tvec.cross(edge1);
        double v = ray.d.dot(qvec) * invDet;
        if (v < 0.0 || u + v > 1.0) return false;

        t = edge2.dot(qvec) * invDet;
        return t > EPS;
    }
        
        // ray-mesh intersection
        struct Intersection hit(Ray ray, double t0, double t1) override;
    };