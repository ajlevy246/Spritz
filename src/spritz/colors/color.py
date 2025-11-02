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
WHITE = Color(1, 1, 1)
GRAY = Color(0.75, 0.75, 0.75)
BLACK = Color(0, 0, 0)
RED = Color(1, 0, 0)
ORANGE = Color(1, 0.5, 0)
YELLOW = Color(1, 1, 0)
GREEN = Color(0, 1, 0)
BLUE = Color(0, 0, 1)
INDIGO = Color(0.294, 0, 0.51)
VIOLET = Color(0.933, 0.51, 0.933)
WHITE = Color(1, 1, 1)