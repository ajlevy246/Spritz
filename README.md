Reference: Fundamentals of Computer Graphics; 5th Edition

# Spritz

Spritz is a simple raytracer implemented in Python, using Numba's JIT compiler for performance.


## How's it work? 

The rendering logic is simple, and looks something like:

```python
pixels = [] #array of RGB values to display

for each pixel on screen:
    #Generate a beam from the camera
    ray = camera.generate_ray(pixel.x, pixel.y)

    #Trace that ray until it hits a surface in the scene
    closest_hit = None
    for each surface in scene:
        surface_hit = surface.hit(ray)
        closest_hit = min(closest_hit, surface_hit)

    #Calculate the color of the surface hit by the ray
    pixel_color = (0.0, 0.0, 0.0) #RGB
    for each light in scene:
        light_contribution = light.illuminate(closest_hit)
        pixel_color += light_contribution

        #further logic for shadows and reflections
        ...

        pixels.append(pixel_color)
```

### 1. Cameras
Every scene starts with a `Camera` object, that is used to calculate the angle of the ray through each pixel. This ray is calculated using only the coordinates of the pixel on the virtual screen, the **aspect ratio** and **fov** of the screen, and the orientation of the camera, characterized by its three basis vectors **u** (up), **v** (right), and **w** (backwards, i.e. towards the viewer). 

### 2. Surfaces
Next, the scene must calculate the color of the ray that is sent out. It loops through every `Surface` in the scene to find the first object hit. Every `Surface` object (e.g, `Sphere`, `Triangle`, `Plane`) must come equipped with a method to calculate whether a given ray intersects with itself. If an intersection is found, it returns **t**, the time that the ray hit it, **n**, the normal vector to the surface at the point of intersection, and a reference to itself.

### 3. Lights
Now that we know what object the ray hit, the color at the point of intersection is computed. For each light in the scene, a ray is beamed towards the point of intersection. If another object is in the way of a given light, then the point is in shadow, and that light makes no contribution. Otherwise, the illumination is computed based on the `Material` of the surface and the type of `Light`. Reflections are computed simply as a recursive call to the shade routine, with the reflected ray as the new beam. 

<div style="display:flex;justify-content:center">
<pre><code class="language-python">
                                      O <span style="color: orange;">- camera eye</span>
                                     / 
                             _______/_
                   \         |       |   <span style="color: orange;">- view screen</span>
                    \        |____/__|
    <span style="color: orange;">reflected ray - </span> \           /
                      \         /
                       \       /  <span style="color: orange;">- original beam</span>
                        \     /
                         \   /
                          \ /           <span style="color: orange;">/ hit surface</span>
---------------------------X---------------------------
    <span style="color: orange;">point of intersection /</span>
</code></pre>
</div>
