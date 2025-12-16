// mesh.cpp

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "surfaces/mesh.h"

void parse_face_vertex(
    const std::string& token,
    int& v, int& vt, int& vn
) {
    v = vt = vn = -1;

    size_t first = token.find('/');
    size_t second = token.find('/', first + 1);

    if (first == std::string::npos) {
        v = std::stoi(token);
    } else {
        v = std::stoi(token.substr(0, first));

        if (second == std::string::npos) {
            vt = std::stoi(token.substr(first + 1));
        } else {
            if (second > first + 1)
                vt = std::stoi(token.substr(first + 1, second - first - 1));
            vn = std::stoi(token.substr(second + 1));
        }
    }
}


// Load mesh from .obj file - scoped to just triangularized meshes
TriangularMesh::TriangularMesh(const std::string& filename, Material* mat) {
    material = mat;

    std::cout << "Loading from file: " << filename << "\n";
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open OBJ file");

    std::vector<Vec3> temp_vertices;
    std::vector<Vec3> temp_normals;

    std::string line;

    // Bounding box vec
    Vec3 minv = Vec3(MAXFLOAT);
    Vec3 maxv = Vec3(-MAXFLOAT);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string tag;
        iss >> tag;

        if (tag == "v") {
            Vec3 v;
            iss >> v.x >> v.y >> v.z;
            // bounding box check
            if (v.x < minv.x) minv.x = v.x;
            if (v.y < minv.y) minv.y = v.y;
            if (v.z < minv.z) minv.z = v.z;
            if (v.x > maxv.x) maxv.x = v.x;
            if (v.y > maxv.y) maxv.y = v.y;
            if (v.z > maxv.z) maxv.z = v.z;
            temp_vertices.push_back(v);
        }
        else if (tag == "vn") {
            Vec3 n;
            iss >> n.x >> n.y >> n.z;
            temp_normals.push_back(n.normalized());
        }
        else if (tag == "f") {
            std::vector<int> v_idx, vn_idx;

            std::string tok;
            while (iss >> tok) {
                int v, vt, vn;
                parse_face_vertex(tok, v, vt, vn);

                auto fix = [&](int idx, int size) {
                    return idx > 0 ? idx - 1 : size + idx;
                };

                v_idx.push_back(fix(v, temp_vertices.size()));
                vn_idx.push_back(vn != -1 ? fix(vn, temp_normals.size()) : -1);
            }

            // triangulate
            for (size_t i = 1; i + 1 < v_idx.size(); ++i) {
                TriangularFace face;
                face.idx0  = v_idx[0];
                face.idx1  = v_idx[i];
                face.idx2  = v_idx[i + 1];
                faces.push_back(face);
            }
        }
        else {
            std::cout << "Unrecognized tag: " << tag << std::endl;
        }
    }
    std::cout << "Setting bb..." << std::endl;
    bounding_box.minv = minv;
    bounding_box.maxv = maxv;
    vertices = std::move(temp_vertices);
    std::cout << "Loaded material successfully" << std::endl;
}


// ray-mesh intersection
struct Intersection TriangularMesh::hit(const Ray& ray, double t0, double t1) {
    Intersection hit; // empty hit record denotes a miss
    double t_min = t1;
    double t;
    if (!bounding_box.intersect(ray, t0, t1)) return hit;
    for (const TriangularFace& face : faces) {
        Vec3& v1 = vertices[face.idx0];
        Vec3& v2 = vertices[face.idx1];
        Vec3& v3 = vertices[face.idx2];

        if (intersect(ray, v1, v2, v3, t)) {
            if (t > t0 && t < t_min) {
                t_min = t;
                hit.t = t;
                hit.surface = this;
                hit.normal = (v2 - v1).cross(v3 - v1).normalized();
                if (hit.normal.dot(ray.d) > 0.0f) {
                    hit.normal = hit.normal * -1.0f; // flip so always forward facing.
                }
            }
        }
    }
    return hit;
}