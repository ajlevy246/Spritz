import numpy as np

class Intersection:
    """Intersections are at the core of the ray tracing algorithm.

    They represent the intersection between a Ray and a Spritz Surface
    """

    def __init__(self, surface, t, normal) -> None:
        """Creates an Intersection of a ray with a Surface at time t.

        Args:
            surface (Surface): object hit by the ray
            t (float): point on the ray at which the intersection occured
            normal (ArrayLike): normal vector to the surface at the point of intersection
        """
        self.surface = surface
        self.t = t
        self.normal = np.array(normal, dtype=float)

    def __repr__(self):
        nx, ny, nz = self.normal
        return f"<Hit at {self.t} with normal ({nx}, {ny}, {nz}>"