"""Test script for debugging"""

from spritz import *
import numpy as np

if __name__ == "__main__":
    tri = Triangle((1, 0, 0), (0, 1, 0), (0, 0, 1), None)   
    intersection = tri.hit((np.array((1, 1, 1), dtype=float), np.array((-1, -1, -1), dtype=float)))

    if intersection is None:
        print("no hit")
    else:
        print(intersection.t)
        print(intersection.normal)
