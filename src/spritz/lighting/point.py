"""Point Lights"""
import numpy as np

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

    def illuminate(self, scene, ray, intersection):
        """Illuminate a point of intersection.

        Computes the contribution of the light to the shading
        at the point of intersection. Uses Lambert's Cosine Law.

        Args:
            ray (Ray): Ray of intersection
            intersection (Intersection): Hit record of intersection
        """
        x = ray.evaluate(intersection.t) # Point of intersection
        l = self.center - x
        # dist = np.linalg.norm(l)
        dist = np.sqrt(np.dot(l, l))
        l /= dist

        n = intersection.normal
        ndotl = np.dot(n, l)
        if ndotl <= 0: #Hit the inside of the surface
            return BLACK
        
        eps = 1e-4 #Small adjustment to avoid hitting the object itself
        shadow_origin = x + eps * n
        shadow_ray = Ray(shadow_origin, l)
        shadow_hit = scene.hit(shadow_ray, 0, dist - eps)
        if shadow_hit is not None: #Surface is in shadow
            return BLACK
        
        E = self.intensity / dist**2
        v = -ray.direction
        k = intersection.surface.material.reflect(l, v, n)
        return k * E