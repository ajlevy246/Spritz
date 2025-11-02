from .light import Light

import numpy as np

class AmbientLight(Light):
    """Ambient lights give a base illumination to all shadows."""
    
    def __init__(self, intensity):
        """Create a new ambient light with intensity I

        Args:
            intensity (Color): Intensity of the ambient light
        """
        self.intensity = intensity

    def illuminate(self, scene, ray, intersection):
        """Computes the contribution of the light to the shading
        at the point of intersection. 

        Args:
            ray (Ray)
            intersection (Intersection)

        Returns:
            Color: Shading at the given pixel.
        """
        material = intersection.surface.material
        return self.intensity * material.ambient