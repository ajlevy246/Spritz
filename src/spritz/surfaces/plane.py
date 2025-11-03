import numpy as np
from numba import njit

from .surface import Surface
from ..raytracing import Intersection

class Plane(Surface):
    """Spritz Plane"""
    def __init__(self, normal, point, material):
        """Creates a plane, defined by a surface normal and a point on the plane.
        
        A point `p` is on the plane if it satisfied `n * (x - p) = 0`, where x is the given point on the plane.
        """
        self.normal = np.array(normal, dtype=float)
        self.point = np.array(point, dtype=float)
        self.material = material

    def hit(self, ray, t0=0, t1=np.inf):
        """An intersection of a ray and a plane occurs at time t when:
        
        `n * (ray_origin + t*ray_direction - x) = 0`, where x is a point on the plane

        Args:
            ray (tuple[np.array]): _description_
            t0 (int, optional): _description_. Defaults to 0.
            t1 (_type_, optional): _description_. Defaults to np.inf.
        """
        ray_origin, ray_direction = ray
        t = Plane._hit(ray_origin, ray_direction, self.normal, self.point, t0, t1)
        if t is None:
            return None
        return Intersection(self, t, self.normal)

    @njit(cache=True)
    def _hit(
        ray_origin: np.array,
        ray_direction: np.array,
        plane_normal: np.array,
        plane_point: np.array,
        t0: float,
        t1: float,
        ) -> float | None:
        """An intersection of a ray and a plane occurs at time t when:
        
        `n * (ray_origin + t*ray_direction - x) = 0`, where x is a point on the plane

        Args:
            ray (tuple[np.array]): _description_
            t0 (int, optional): _description_. Defaults to 0.
            t1 (_type_, optional): _description_. Defaults to np.inf.
        """
        denom = np.dot(plane_normal, ray_direction)
        if np.abs(denom) < 1e-8: #if the ray is parallel to the plane, no intersection
            return None
        t = np.dot(plane_normal, plane_point - ray_origin) / denom
        if t0 <= t <= t1:
            return t
        return None