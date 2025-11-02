"""Scenes are the 3D environment to be rendered."""
import numpy as np

from ..camera import *
from ..colors import *
from ..lighting import *
from ..materials import *
from ..raytracing import *
from ..surfaces import *

from numba import jit

class Scene:
    """Scenes host all of the objects needed for rendering"""

    def __init__(self, objects=None, lights=None, camera=None, background_color=GRAY):
        self.objects = SurfaceGroup(objects)
        self.lights = lights or []
        self.camera = camera
        self.background_color = background_color
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
                pixels[y, x] = self._shade(ray).rgb

        return pixels

        # pixels = []
        # for row in range(height):
        #     pixel_row = []
        #     for col in range(width):
        #         ray = self.camera.generate_ray(col, row, width, height)
        #         pixel = self._shade(ray).rgb
        #         pixel_row.append(pixel)
        #     pixels.append(pixel_row)
        # return pixels

    def hit(self, ray, t0=0, t1=np.inf):
        return self.objects.hit(ray, t0, t1)

    def _shade(self, ray):
        """Compute pixel for a given viewing ray"""
        intersection = self.hit(ray)
        if intersection is None:
            return self.background_color
        
        shade = Color(0, 0, 0)
        for light in self.lights:
            shade += light.illuminate(self, ray, intersection)
        return shade
