from abc import ABC, abstractmethod

class Light(ABC):
    """Abstract base class of all Lighting objects"""
    
    @abstractmethod
    def illuminate(self, scene, ray, intersection):
        """Compute the illumination of the ray at a surface
        it has hit.

        Args:
            ray (Ray): The ray of intersection
            intersection (Intersection): The hit record of the surface intersected.

        Returns: A Color representing the illumination at the point of intersection.
        """
        ...