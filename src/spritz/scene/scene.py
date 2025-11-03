"""Scenes are the 3D environment to be rendered."""
import numpy as np

from ..camera import *
from ..colors import *
from ..lighting import *
from ..materials import *
from ..raytracing import *
from ..surfaces import *

class Scene:
    """Scenes host all of the objects needed for rendering"""

    def __init__(self, objects=None, lights=None, camera=None, background_color=GRAY, max_bounces=1):
        self.objects = SurfaceGroup(objects)
        self.lights = lights or []
        self.camera = camera
        self.background_color = background_color
        self.max_bounces = max_bounces
        if camera is None:
            self.camera = Camera((1, 1, 1), (-1, -1, -1))

    def add_surface(self, surface):
        self.objects.add_surface(surface)

    def add_light(self, light):
        self.lights.append(light)

    def change_camera(self, camera):
        self.camera = camera

    def render(self, width=50, height=50):
        """Rendering routine for the scene's camera."""
        origins, directions = self.camera.generate_rays(width, height)

        pixels = np.zeros((height, width, 3), dtype=np.float64)

        for y in range(height):
            for x in range(width):
                ray = (origins[y, x], directions[y, x])
                pixels[y, x] = self._shade(ray)

        return pixels

    def hit(self, ray, t0=0, t1=np.inf):
        return self.objects.hit(ray, t0, t1)

    def _shade(self, ray, bounces=0):
        """Compute pixel for a given viewing ray"""
        intersection = self.hit(ray)
        if intersection is None:
            return self.background_color
        
        shade = np.array((0, 0, 0), dtype=float)
        for light in self.lights:
            light_contribution = light.illuminate(self, ray, intersection)
            shade += light_contribution

        # reflect
        if bounces < self.max_bounces:
            ray_origin, ray_direction = ray
            reflect_origin = ray_origin + intersection.t * ray_direction + 1e-6 * intersection.normal
            reflect_direction = ray_direction - 2*(np.dot(ray_direction, intersection.normal))*intersection.normal
            reflect_ray = (reflect_origin, reflect_direction)
            shade += np.multiply(intersection.surface.material.specular, self._shade(reflect_ray, bounces + 1))
            
        return shade
