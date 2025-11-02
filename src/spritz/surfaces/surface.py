"""Core module for Spritz surfaces."""
from abc import ABC, abstractmethod
import numpy as np

from ..raytracing import Intersection

class Surface(ABC):
    """Surface object is the parent class of all surfaces in the engine."""
    
    @abstractmethod
    def hit(self, ray, t0=0, t1=np.inf) -> Intersection:
        """Returns the Intersection of the surface with a ray.

        If the two do not intersect, return None

        Args:
            ray (Ray): Ray to check intersection
            t0 (float): time start
            t1 (float): time end
        """
        ...

class SurfaceGroup(Surface):
    """A group of surfaces. Allows for easy ray-object intersections."""

    def __init__(self, surfaces=None):
        """Create a new surface group with the given surfaces.

        Args:
            surfaces (list[Surface], optional): Initial list of surfaces. Defaults to None.
        """
        if surfaces is None:
            self.surfaces = list()
        else:
            self.surfaces = surfaces

    def add_surface(self, surface):
        self.surfaces.append(surface)

    # def __repr__(self):
    #     return f"<Surface Group>\n{'  -'.join([surface.__repr__() for surface in self.surfaces])}"
    
    def hit(self, ray, t0=0, t1=np.inf) -> Intersection:
        """_summary_

        Args:
            ray (Ray): Ray to check intersection
            t0 (float, optional): time start. Defaults to 0.
            t1 (float, optional): time end. Defaults to np.inf.
        """
        closest_hit = None
        for surface in self.surfaces:
            curr_hit = surface.hit(ray, t0, t1)
            if curr_hit is None:
                continue
            closest_hit = curr_hit
            t1 = closest_hit.t
        return closest_hit