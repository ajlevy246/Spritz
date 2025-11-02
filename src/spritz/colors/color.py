"""Simple representation of an RGB color"""

import numpy as np

class Color:
    """Color utility for converting from RGB to hex and uint8"""

    def __init__(self, r, g, b):
        """_summary_

        Args:
            r (float): Red
            g (float): Green
            b (float): Blue
        """
        self.rgb = np.array((r, g, b), dtype=float)

    def __add__(self, other):
        return Color(*(self.rgb + other.rgb))
    
    def __mul__(self, other):
        if isinstance(other, Color): 
            # Componentwise multiplication
            return Color(*(self.rgb * other.rgb))
        else:
            # Scalar multiplication
            return Color(*(self.rgb * other))
        
    __rmul__ = __mul__ # For scalar multiplication

    def __truediv__(self, other):
        if isinstance(other, Color):
            raise ValueError("Not Implemented")
        else:
            return Color(*(self.rgb / other))

    def __repr__(self):
        r, g, b = self.rgb
        return f"<Color: ({r}, {g}, {b})>"
    
# Basic colors
# WHITE = np.array((1, 1, 1), dtype=float)
# GRAY = np.array((0.75, 0.75, 0.75), dtype=float)
# BLACK = np.array((0, 0, 0), dtype=float)
# RED = np.array((1, 0, 0), dtype=float)
# ORANGE = np.array((1, 0.5, 0), dtype=float)
# YELLOW = np.array((1, 1, 0), dtype=float)
# GREEN = np.array((0, 1, 0), dtype=float)
# BLUE = np.array((0, 0, 1), dtype=float)
# INDIGO = np.array((0.294, 0, 0.51), dtype=float)
# VIOLET = np.array((0.933, 0.51, 0.933), dtype=float)
# WHITE = np.array((1, 1, 1), dtype=float)

WHITE = (1, 1, 1)
GRAY = (0.75, 0.75, 0.75)
BLACK = (0, 0, 0)
RED = (1, 0, 0)
ORANGE = (1, 0.5, 0)
YELLOW = (1, 1, 0)
GREEN = (0, 1, 0)
BLUE = (0, 0, 1)
INDIGO = (0.294, 0, 0.51)
VIOLET = (0.933, 0.51, 0.933)
WHITE = (1, 1, 1)