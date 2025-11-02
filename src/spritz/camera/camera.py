"""Cameras are the source of the viewing rays used to render a scene.

Orientation: 'up' is the positive z-axis.
    - u: right of the viewing direction.
    - w: vector from center of viewframe to the eye ('backwards')
    - v: 'up' the viewframe (coplanar with w and standard up vector)
"""
import numpy as np

from ..raytracing import Ray

class Camera: 
    """For now, just a barebones pinhole camera"""
    def __init__(self, eye, direction, up=(0, 0, 1), aspect=1.0):
        """Build a Camera with eye e and direction d. 

        Args:
            eye (ArrayLike): Camera eye
            direction (ArrayLike): Viewing direction
            up (tuple, optional): Scene 'up' vector. Defaults to (0, 0, 1).
            aspect (float, optional): aspect ratio of camera. Defaults to 1.0.
        """
        self.eye = np.array(eye, dtype=float)
        self.aspect = aspect

        # Build coordinate frame
        w = (self.eye - np.array(direction, dtype=float))
        w /= np.linalg.norm(w)
        u = np.cross(np.array(up, dtype=float), w)
        u /= np.linalg.norm(u)
        v = np.cross(w, u)

        self.u, self.v, self.w = u, v, w

    def generate_ray(self, x, y, width, height):
        """
        Generate a ray through pixel (x, y) assuming the image plane
        spans [-1, 1] in Y and [-aspect, aspect] in X.
        """
        # Normalize coords
        px = (2 * (x + 0.5) / width - 1) * self.aspect
        py = (1 - 2 * (y + 0.5) / height)

        # Calculate ray direction
        direction = (px * self.u + py * self.v - self.w)
        # direction /= np.linalg.norm(direction)
        direction /= np.sqrt(np.dot(direction, direction))

        return Ray(self.eye, direction)
