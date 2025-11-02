import numpy as np

from .surface import Surface
from ..raytracing import Ray, Intersection

class Triangle(Surface):
    """Spritz Triangle"""

    def __init__(self, a, b, c, material):
        """Triangle with vertices a, b, c in clock-wise orientation.

        Args:
            a (ArrayLike): 1st vertex
            b (ArrayLike): 2nd vertex
            c (ArrayLike): 3rd vertex
        """
        self.v1 = np.array(a, dtype=float)
        self.v2 = np.array(b, dtype=float)
        self.v3 = np.array(c, dtype=float)
        self.material = material

    def __repr__(self):
        v1 = f"({self.v1[0]}, {self.v1[1]}, {self.v1[2]})"
        v2 = f"({self.v2[0]}, {self.v2[1]}, {self.v2[2]})"
        v3 = f"({self.v3[0]}, {self.v3[1]}, {self.v3[2]})"
        return f"<Triangle at {v1}->{v2}->{v3}>"

    def hit(self, ray, t0=0, t1=np.inf):
        """An intersection of a ray with the triangle occurs at time t if:

        `ray_origin` + `t`*`ray_direction` = `v1` + `\beta(v2 - v1)` + `gamma(v3 - 1)`

        This is a linear system solvable with Cramer's rule.
        
        Args:
            ray (Ray): Ray to check
            t0 (float): Start of time interval
            t1 (float): End of time interval

        Returns: None if no hit, otherwise Intersection object
        """
        # Build linear system:
        # [a d g] [\beta ]    [j]
        # |b e h| |\gamma| =  |k|
        # [c f i] [  t   ]    [l]
        v2v1 = self.v1 - self.v2 # Edge v2 -> v1
        v3v1 = self.v1 - self.v3 # Edge v3 -> v1
        a, b, c = v2v1 
        d, e, f = v3v1
        g, h, i = ray.direction
        j, k, l = self.v1 - ray.origin # Vector from ray origin to v1

        # Solve with Cramer's rule
        ei_hf = e*i - h*f
        gf_di = g*f - d*i
        dh_eg = d*h - e*g
        det = a*ei_hf + b*gf_di + c*dh_eg
        if det == 0:
            print("M fails")
            return None
        
        ak_jb = a*k - j*b
        jc_al = j*c - a*l
        bl_kc = b*l - k*c

        # Early stopping conditions
        t = -(f*ak_jb + e*jc_al + d*bl_kc) / det
        if t0 <= t <= t1:
            gamma = (i*ak_jb + h*jc_al + g*bl_kc) / det
            if 0 <= gamma <= 1:
                beta = (j*ei_hf + k*gf_di + l*dh_eg) / det
                if 0 <= beta <= 1 - gamma:
                    normal = np.cross(v2v1, v3v1)
                    normal /= np.linalg.norm(normal)
                    return Intersection(self, t, normal)
        return None

                
        