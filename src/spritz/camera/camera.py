"""Cameras are the source of the viewing rays used to render a scene.

Orientation: 'up' is the positive z-axis.
    - u: right of the viewing direction.
    - w: vector from center of viewframe to the eye ('backwards')
    - v: 'up' the viewframe (coplanar with w and standard up vector)
"""
import numpy as np
from numba import njit

class Camera: 
    """For now, just a barebones pinhole camera"""
    def __init__(self, eye, direction, up=(0, 0, 1), aspect=1.0, fov=114):
        """Build a Camera with eye e and direction d. 

        Args:
            eye (ArrayLike): Camera eye
            direction (ArrayLike): Viewing direction
            up (tuple, optional): Scene 'up' vector. Defaults to (0, 0, 1).
            aspect (float, optional): aspect ratio of camera. Defaults to 1.0.
            fov (int, optional): field of view in degrees. Defaults to 114
        """
        self.eye = np.array(eye, dtype=float)
        self.aspect = aspect
        self.fov = fov

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
        direction = Camera._generate_ray(
            self.u,
            self.v,
            self.w,
            x,
            y,
            width,
            height, 
            self.aspect,
        )

        return (self.eye, direction)

    @njit(cache=True)
    def _generate_ray(u, v, w, x, y, wid, hei, a):
        px = (2 * (x + 0.5) / wid - 1) * a
        py = (1 - 2 * (y + 0.5) / hei)

        direction = (px * u + py * v - w)
        direction /= np.sqrt(np.dot(direction, direction))
        return direction
    
    def generate_rays(self, width, height):
        return Camera._generate_rays(
            width,
            height,
            self.eye,
            self.fov,
            self.aspect,
            self.u,
            self.v,
            self.w,
        )

    @njit(cache=True)
    def _generate_rays(width: int,
                    height: int,
                    eye: np.ndarray,
                    fov: float,
                    aspect: float,
                    u: np.ndarray,
                    v: np.ndarray,
                    w: np.ndarray):
        
        fov = np.deg2rad(fov)

        # Image plane half-size
        half_height = np.tan(fov / 2.0)
        half_width = aspect * half_height

        # Pixel centers in normalized coords
        px = (2.0 * (np.arange(width) + 0.5) / width - 1.0) * half_width
        py = (1.0 - 2.0 * (np.arange(height) + 0.5) / height) * half_height

        # Allocate arrays
        dirs = np.zeros((height, width, 3), dtype=np.float64)
        origins = np.zeros((height, width, 3), dtype=np.float64)

        # Fill in each direction
        for j in range(height):
            for i in range(width):
                # Compute direction for this pixel
                dir_vec = px[i] * u + py[j] * v - w

                # Normalize
                norm = np.sqrt(dir_vec[0]**2 + dir_vec[1]**2 + dir_vec[2]**2)
                dirs[j, i, 0] = dir_vec[0] / norm
                dirs[j, i, 1] = dir_vec[1] / norm
                dirs[j, i, 2] = dir_vec[2] / norm

                # Copy camera origin
                origins[j, i, 0] = eye[0]
                origins[j, i, 1] = eye[1]
                origins[j, i, 2] = eye[2]

        return origins, dirs