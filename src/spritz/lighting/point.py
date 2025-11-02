"""Point Lights"""
import numpy as np
from numba import njit

from .light import Light
from ..raytracing import Ray
from ..colors import Color, BLACK

class PointLight(Light):
    """Point Lights are single points that illuminate
    in all directions.
    """
    def __init__(self, center, intensity):
        """Create a new PointLight at point o and color c.

        Args:
            center (ArrayLike): Coordinates of the point
            intensity (Color): intensity of the light
        """
        self.center = np.array(center, dtype=float)
        self.intensity = intensity

    # def illuminate(self, scene, ray, intersection):
    #     """Illuminate a point of intersection.

    #     Computes the contribution of the light to the shading
    #     at the point of intersection. Uses Lambert's Cosine Law.

    #     Args:
    #         ray (Ray): Ray of intersection
    #         intersection (Intersection): Hit record of intersection
    #     """
    #     ray_origin, ray_direction = ray
    #     illumination = PointLight._illuminate(ray_origin, ray_direction, intersection.t, intersection.normal, self.center)
    #     if illumination is None:
    #         return BLACK
        
    #     shadow_origin, dist, eps, l = illumination
    #     shadow_ray = (shadow_origin, l)
    #     shadow_hit = scene.hit(shadow_ray, 0, dist - eps)
    #     if shadow_hit is not None: #Surface is in shadow
    #         return BLACK
        
    #     E = self.intensity / dist**2
    #     v = -ray_direction
    #     k = intersection.surface.material.reflect(l, v, intersection.normal)
    #     return k * E
    
    # @njit
    # def _illuminate(
    #         ray_origin: np.array,
    #         ray_direction: np.array,
    #         intersection_t: float,
    #         intersection_normal: np.array,
    #         light_origin: np.array,            
    # ) -> None | tuple[float, np.array]:
    #     x = ray_origin + intersection_t * ray_direction # Point of intersection
    #     l = light_origin - x
    #     dist = np.sqrt(np.dot(l, l))
    #     l /= dist

    #     n = intersection_normal
    #     ndotl = np.dot(n, l)
    #     if ndotl <= 0:
    #         return None
        
    #     eps = 1e-4
    #     shadow_origin = x + eps * n
    #     return (shadow_origin, dist, eps, l)

    def illuminate(self, scene, ray, intersection):
        """Illuminate a point of intersection.

        Computes the contribution of the light to the shading
        at the point of intersection. Uses Lambert's Cosine Law.

        Args:
            ray (Ray): Ray of intersection
            intersection (Intersection): Hit record of intersection
        """
        ray_origin, ray_direction = ray
        illumination = PointLight._illuminate(ray_origin, ray_direction, intersection.t, intersection.normal, self.center)
        if illumination is None:
            return BLACK
        
        shadow_origin, dist, eps, l = illumination
        shadow_ray = (shadow_origin, l)
        shadow_hit = scene.hit(shadow_ray, 0, dist - eps)
        if shadow_hit is not None: #Surface is in shadow
            return BLACK
        
        E = self.intensity / dist**2
        v = -ray_direction
        k = intersection.surface.material.reflect(l, v, intersection.normal)
        return k * E
    
    @njit(cache=True)
    def _illuminate(
            ray_origin: np.array,
            ray_direction: np.array,
            intersection_t: float,
            intersection_normal: np.array,
            light_origin: np.array,            
    ) -> None | tuple[float, np.array]:
        x = ray_origin + intersection_t * ray_direction # Point of intersection
        l = light_origin - x
        dist = np.sqrt(np.dot(l, l))
        l /= dist

        n = intersection_normal
        ndotl = np.dot(n, l)
        if ndotl <= 0:
            return None
        
        eps = 1e-4
        shadow_origin = x + eps * n
        return (shadow_origin, dist, eps, l)
        