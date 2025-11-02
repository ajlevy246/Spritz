import numpy as np
from numba import njit

from .surface import Surface
from ..raytracing import Intersection

class Sphere(Surface):
    """Spritz Sphere"""

    def __init__(self, c, r, material):
        """Sphere with center c and radius r

        Args:
            c (ArrayLike): center
            r (float): radius
        """
        self.center = np.array(c, dtype=float)
        self.radius = r
        self.material = material

    def __repr__(self):
        return f"<Sphere at({self.center[0]},{self.center[1]},{self.center[2]}) and r={self.radius}>"

    def hit(self, ray, t0=0, t1=np.inf):
        ray_origin, ray_direction = ray
        t = Sphere._hit(self.center, self.radius, ray_origin, ray_direction, t0, t1)
        if t is None:
            return None
        
        t, normal = t
        return Intersection(self, t, normal)

    @njit(cache=True)
    def _hit(
        sphere_center: np.array,
        sphere_radius: float, 
        ray_origin: np.array,
        ray_direction: np.array,
        t0: float, 
        t1: float) -> tuple[float, np.array] | None:
        """An intersection of a ray with the sphere occurs at time t if:

        |(`ray_origin` + `t`*`ray_direction` - `sphere_center`)| - `sphere_radius`^2 = 0

        This is a quadratic: `Ax^2 + Bx + C`.
        
        Args:
            ray (Ray): Ray to check
            t0 (float): Start of time interval
            t1 (float): End of time interval

        Returns: None if no hit, otherwise Intersection object
        """
        dist = ray_origin - sphere_center
        A = np.dot(ray_direction, ray_direction)
        B = np.dot(ray_direction, dist) 
        C = np.dot(dist, dist) - sphere_radius**2

        discriminant = B**2 - A*C
        if discriminant < 0: # No intersection
            return None
        
        dsqrt = np.sqrt(discriminant)
        h_1 = (-B + dsqrt) / A 
        h_2 = (-B - dsqrt) / A
        
        if t0 <= h_1 <= t1:
            if t0 <= h_2 <= t1:
                h = np.minimum(h_1, h_2)
            else:
                h = h_1
        else:
            if t0 <= h_2 <= t1:
                h = h_2
            else: # Neither intersection point lies in the time interval we care about
                  # i.e., if another object is in between this sphere and the ray's origin
                return None 
                
        intersection_point = ray_origin + h * ray_direction
        normal = (intersection_point - sphere_center) / sphere_radius
        return (h, normal)