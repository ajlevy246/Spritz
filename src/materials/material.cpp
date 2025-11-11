#include "materials.h"
#include "scenes.h"

#include <algorithm>
#include <cstdio>

    // // ===== MEMBERS =====
    // Vec3 ka, kd, ks; 
    // int shininess;
    // double ior;
    // Vec3 filter;

// ===== CONSTRUCTORS =====
Material::Material() {
    ka = Vec3(0.1, 0.1, 0.1);
    kd = Vec3(0.1, 0.1, 0.1);
    ks = Vec3(0.1, 0.1, 0.1);
    shininess = 32;
    ior = 1.2;
    filter = 0.8;
    reflection = 0.2;
    filter_color = Vec3(0.1, 0.2, 0.1);
}
Material::Material(Vec3 ka_, Vec3 kd_, Vec3 ks_, int shininess_, double ior_, double filter_, double reflection_, Vec3 filter_color_) {
    ka = ka_;
    kd = kd_;
    ks = ks_;
    shininess = shininess_;
    ior = ior_;
    filter = filter_;
    reflection = reflection_;
    filter_color = filter_color_;
}

// ===== METHODS =====

// Compute blinn_phong specular reflection and Lambertian diffusion.
Vec3 Material::blinn_phong(const Vec3& surface_normal, const Vec3& light_direction, const Vec3& viewing_direction) const {
    Vec3 diffusion_comp = Vec3(kd) * (surface_normal.dot(light_direction));
    Vec3 half_angle = (Vec3(viewing_direction) + light_direction).normalized();
    double ndoth = std::max(0.0, surface_normal.dot(half_angle));
    Vec3 specular_comp = Vec3(ks) * pow(ndoth, shininess);
    return diffusion_comp + specular_comp;
}

double Material::shlick_approx(const Vec3& surface_normal, const Vec3& viewing_direction) const {
    double cos_theta = fabs(surface_normal.dot(viewing_direction));
    double r_0 = pow((1 - ior) / (1 + ior), 2);
    return r_0 + (1 - r_0) * pow(1 - cos_theta, 5);
}

Vec3 Material::shade(const Scene& scene, const Ray& ray, const Intersection& hit, int bounce) const {
    if (bounce > MAX_BOUNCES) return Vec3(0, 0, 0);

    Vec3 x = ray.evaluate(hit.t);
    Vec3 n = hit.normal;
    Vec3 v = ray.d * -1;

    // ----- Local Illumination -----
    Vec3 local_color = Vec3(0);
    for (const auto& light: scene.lights) {
        Vec3 l = light->direction_from(x);
        double dist = light->distance_from(x);
        Vec3 intensity = light->intensity_at(x);

        // Cast shadow ray (accumulate transparency)
        Vec3 shadow_origin = x + l * EPS;
        Ray shadow_ray = Ray(shadow_origin, l);

        // Trace through all intersections between x and the light. 
        //  - If the intermediate objets are transparent, light gets through
        Vec3 light_attenuation = Vec3(1);
        double current_t = 0.0;
        while (current_t < dist - EPS) {
            Intersection shadow_hit = scene.hit(shadow_ray, current_t + EPS, dist - EPS);

            if (shadow_hit.t >= dist - EPS) {
                break; // no more hits, the light has finally reached x
            }

            const Material* blocking_material = shadow_hit.surface->material;

            if (blocking_material && blocking_material->filter > 0.0) {
                // Interfering object is transparent
                Vec3 transparency = Vec3(blocking_material->filter_color) * double(blocking_material->filter);
                light_attenuation = light_attenuation * transparency;

                // Round down if the object is barely transparent
                if (light_attenuation.length_sq() < 0.05) {
                    light_attenuation = Vec3(0);
                    break;
                }
            } else {
                // Object is opaque, so x is in shadow
                light_attenuation = Vec3(0);
                break;
            }

            // iterate to the next intersection
            current_t = shadow_hit.t;
        }

        if (light_attenuation.length_sq() > EPS) {
            local_color = local_color + (intensity * light_attenuation) * blinn_phong(n, l , v);
        }
    }

    local_color = local_color + ka;
    
    
    
    
    // // ----- Local Illumination -----
    // Vec3 local_color = Vec3(0, 0, 0);
    // for (const auto& light : scene.lights) {
    //     Vec3 l = light->direction_from(x);
    //     double dist = light->distance_from(x);
    //     Vec3 intensity = light->intensity_at(x);

    //     // Cast shadow ray
    //     Vec3 shadow_origin = x + l * EPS;
    //     Ray shadow_ray = Ray(shadow_origin, l);
    //     Intersection shadow_hit = scene.hit(shadow_ray, 0, dist - EPS);
    //     bool in_shadow = shadow_hit.t < dist - EPS;

    //     if (!in_shadow) {
    //         local_color = local_color + intensity * blinn_phong(n, l, v);
    //     }
    // }

    // local_color = local_color + ka;

    // ----- Reflection / Refraction -----
    Vec3 reflect_direction = (ray.d - n * 2 * ray.d.dot(n)).normalized();
    Vec3 reflect_origin = x + reflect_direction * EPS;
    Ray reflection_ray = Ray(reflect_origin, reflect_direction);
    Vec3 reflection_color = scene.shade_ray(reflection_ray, bounce + 1);

    Vec3 transmission_color = Vec3(0);
    if (filter > 0.0) {
        // Determine if the ray is entering or exiting
        Vec3 nn = n;
        double cosi = std::clamp(ray.d.dot(n), -1.0, 1.0);
        double n1 = 1.0;     // index of refraction of air
        double n2 = ior;
        double eta = n1 / n2;

        // If the ray is inside the object (cosi > 0), flip the normal and invert eta
        if (cosi > 0.0) {
            nn = n * -1;
            std::swap(n1, n2);
            eta = n1 / n2;
        }

        // Compute refraction using Snell's law
        double cos_theta_i = -std::clamp(ray.d.dot(nn), -1.0, 1.0);
        double k = 1.0 - eta * eta * (1.0 - cos_theta_i * cos_theta_i);

        if (k < 0.0) {
            // Total internal reflection — no transmitted ray
            // (You could optionally add this energy to reflection)
        } else {
            Vec3 refracted_direction = ray.d * eta + nn * (eta * cos_theta_i - sqrt(k));
            refracted_direction = refracted_direction.normalized();
            Vec3 refracted_origin = x + refracted_direction * EPS;
            Ray refracted_ray(refracted_origin, refracted_direction);
            transmission_color = filter_color * scene.shade_ray(refracted_ray, bounce + 1);
        }
    }

    // ----- Fresnel -----
    double fresnel = shlick_approx(n, v);
    double local_contribution = (1 - filter) * (1 - reflection); 
    double reflection_contribution = (1 - filter) * reflection + filter * fresnel;
    double transmission_contribution = filter * (1 - fresnel);

    Vec3 total = (local_color * local_contribution) + 
        (reflection_color * reflection_contribution) + 
        (transmission_color * transmission_contribution);

    return total;
}