"""Materials are responsible for computing BRDF values at pixel."""
from abc import ABC, abstractmethod
import numpy as np

from ..colors import *

class Material():

    """Base class for all Materials."""
    def __init__(self, ambient, diffuse, specular, shininess=0):
        """Creates a new Material with ka, kd, and ks as coefficients.

        Ambient Coefficient: R/pi, where R is the reflectance (fraction of irradiance it reflects)

        Args:
            ambient_coeff (Color): Ambient reflectance coefficient
            diffuse_coeff (Color): Diffuse reflectance coefficient
            specular_coeff (Color): Specular reflectance coefficient
            shininess (int), optional: Phong exponent
        """
        self.ambient = ambient
        self.diffuse = diffuse
        self.specular = specular
        self.shininess = shininess

    def reflect(self, light_direction, viewing_direction, surface_normal):
        """Compute the BRDF value contributed by this material when 
        a light source hits it. Light source is quantified by three vectors:

        Args:
            light_direction (ArrayLike): Direction of light
            viewing_direction (ArrayLike): Direction pointing towards viewing ray origin.
            surface_normal (ArrayLike): Surface normal at the point of intersection.
        """
        # Lambertian Reflection
        diffusion_comp = np.maximum(0, np.dot(surface_normal, light_direction)) * self.diffuse

        # Specular (Binn-Phong)
        half_vector = light_direction + viewing_direction
        # half_vector /= np.linalg.norm(half_vector)
        half_vector /= np.sqrt(np.dot(half_vector, half_vector))
        specular_comp = self.specular * np.power(np.maximum(0, np.dot(surface_normal, half_vector)), self.shininess)

        return diffusion_comp + specular_comp