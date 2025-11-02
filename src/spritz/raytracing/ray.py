import numpy as np

class Ray:
    """Implementation of a simple parametric three-dimensional ray."""

    def __init__(self, origin: list[float], direction: list[float]) -> None:
        """Create a new parametrized Ray.

        Args:
            origin (ArrayLike): Ray's point of origin
            direction (ArrayLike): Ray's direction vector
        """
        self.origin = np.array(origin, dtype=float)
        self.direction = np.array(direction, dtype=float)
        
        # norm = np.linalg.norm(self.direction)
        norm = np.sqrt(np.dot(self.direction, self.direction))
        if norm == 0:
            raise ValueError("Direction vector should not be zero")
        
        self.direction /= norm

    def __repr__(self):
        ox, oy, oz = self.origin
        dx, dy, dz = self.direction
        return f"<Ray at ({ox}, {oy}, {oz}) to ({dx}, {dy}, {dz})>"

    def evaluate(self, t: float) -> np.array:
        """Evaluate the point at time t.

        Args:
            t (float): time to evaluate the line at

        Returns:
            NDArray: point on the line at time t
        """
        return self.origin + t*self.direction
