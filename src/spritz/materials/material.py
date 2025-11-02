"""Materials are responsible for computing BRDF values at pixel."""
from abc import ABC, abstractmethod
import numpy as np

from numba import njit

from ..colors import *

class Material():

    """Base class for all Materials."""
    def __init__(self, ambient, diffuse, specular, shininess=0):
        """Creates a new Material with ka, kd, and ks as coefficients.

        Ambient Coefficient: R/pi, where R is the reflectance (fraction of irradiance it reflects)

        Args:
            ambient_coeff (tuple[float]): Ambient reflectance coefficient   (RGB)
            diffuse_coeff (tuple[float]): Diffuse reflectance coefficient   (RGB)
            specular_coeff (tuple[float]): Specular reflectance coefficient (RGB)
            shininess (int), optional: Phong exponent
        """
        self.ambient = np.array(ambient, dtype=float)
        self.diffuse = np.array(diffuse, dtype=float)
        self.specular = np.array(specular, dtype=float)
        self.shininess = shininess

    def reflect(self, light_direction, viewing_direction, surface_normal):
        """Compute the BRDF value contributed by this material when 
        a light source hits it. Light source is quantified by three vectors:

        Args:
            light_direction (ArrayLike): Direction of light
            viewing_direction (ArrayLike): Direction pointing towards viewing ray origin.
            surface_normal (ArrayLike): Surface normal at the point of intersection.
        """
        
        color = Material._reflect(
            np.array(self.diffuse, dtype=float),
            np.array(self.specular, dtype=float),
            light_direction,
            viewing_direction,
            surface_normal,
            self.shininess,
        )
        return color
    
    @njit(cache=True)
    def _reflect(kd: np.array, ks: np.array, l: np.array, v: np.array, n: np.array, shininess: int) -> np.array:

        # Lambertian Reflection
        diffusion_comp = np.maximum(0, np.dot(n, l)) * kd

        half_vector = l + v
        half_vector /= np.sqrt(np.dot(half_vector, half_vector))
        specular_comp = ks * np.power(np.maximum(0, np.dot(n, half_vector)), shininess)

        return diffusion_comp + specular_comp