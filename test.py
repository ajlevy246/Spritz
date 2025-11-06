"""Test Scenes until real scene is created."""

"""Times (1000x1000, 2spheres, 1point, 1ambient):
== FIRST == SECOND == CHANGE
- 25.54,    ...   - No optimizations
- 22.57,    ...   - Removed calls to np.linalg.norm()
- 22.25,    ...   - JIT for sphere intersections
- 18.33,   12.50  - NJIT for triangle and sphere intersections - removed Ray
- 17.85,   12.55  - NJIT for normal calculations
- 23.97,    5.33  - NJIT for point light illumination (weird one-off)
- 17.11,   10.72  - NJIT for Material reflections (diffusion and specular)
- 13.61,    5.80  - NJIT for single ray generation
- 10.87,    6.27S - Added NJIT caching to disk (speed-up first run)
- 10.33,    3.82  - Vectorized ray generation entirely, with NJIT
- 4.08,     4.49  - With ~6.5 second pre-render (to compile math routines)
Current Average: 5.98 seconds

Now with reflections (1 bounce) {500px x 500px}
== AVERAGE (3) == CHANGE
- 6.45           - Base
- 6.30           - Parallelization for Camera's _generate_rays()
"""
TEST = True
ANIMATE = False
PROFILE = False

import time

script_start = time.time()

import_start = time.time()
from spritz import *
from PIL import Image
import numpy as np
if PROFILE:
    import cProfile, pstats
import_end = time.time()
print(f"imported modules in {import_end - import_start:.2f} seconds")

# ========== Sample Materials =========
shiny = Material(
    ambient=(0.3, 0.3, 0.3),
    diffuse=(0.5, 0.5, 0.5),
    specular=(1, 1, 1),
    shininess=64,
)
shiny_plane = Material(
    ambient=(0.3, 0.3, 0.3),
    diffuse=(1, 1, 1),
    specular=(1, 1, 1),
    shininess=64
)
black_plane = Material(
    ambient=(0, 0, 0),
    diffuse=(0.025, 0.025, 0.025),
    specular=(0.1, 0.1, 0.1),
    shininess=8
)
blank = Material(
    ambient=BLACK,
    diffuse=BLACK,
    specular=BLACK,
    shininess=0,
)
red_matte = Material(
    ambient=(0.1, 0.0, 0.0),
    diffuse=(0.8, 0.1, 0.1),
    specular=(0.0, 0.0, 0.0),
    shininess=0,
)

yellow_plastic = Material(
    ambient=(0.1, 0.1, 0.0),
    diffuse=(0.9, 0.9, 0.1),
    specular=(0.4, 0.4, 0.2),
    shininess=32,
)

gold_metal = Material(
    ambient=(0.05, 0.04, 0.0),
    diffuse=(0.83, 0.68, 0.21),
    specular=(0.9, 0.8, 0.4),
    shininess=128,
)
silver_metal = Material(
    ambient=(0.4, 0.4, 0.4),
    diffuse=(0.7, 0.7, 0.7),
    specular=(0.9, 0.9, 0.9),
    shininess=128,
)
blue_mirror = Material(
    ambient=(0.0, 0.0, 0.1),
    diffuse=(0.0, 0.0, 0.2),
    specular=(1.0, 1.0, 1.0),
    shininess=256
)
white_glossy = Material(
    ambient=(0.1, 0.1, 0.1),
    diffuse=(0.8, 0.8, 0.8),
    specular=(0.5, 0.5, 0.5),
    shininess=64,
)

# ========== Build Scene =========
scene = Scene(background_color=BLACK)

# Camera
camera = Camera(
    eye=(4, 8, 1.5),
    direction=(-1, -3, -0.5),
)
scene.change_camera(camera)

# Lights
point_light = PointLight(
    center=(10, 3, 0),
    intensity=(25, 25, 25)
)
point_b = PointLight(
    center = (-2, 0, 5),
    intensity = (25, 25, 25),
)
ambient_light = AmbientLight(
    intensity=(0.5, 0.5, 0.5),
)
scene.add_light(point_light)
scene.add_light(point_b)
scene.add_light(ambient_light)

# Surfaces
sphere_a = Sphere(
    c=(0, 0, 0),
    r=2,
    material=red_matte,
)
sphere_b = Sphere(
    c=(5, 0, 0),
    r=2,
    material=gold_metal,
)
tri = Triangle(
    a=(-3, 2, 3),
    c=(-1.5, 5, 5),
    b=(-2, 7, 2),
    material=blue_mirror,
)
plane = Plane(
    normal=(0, 0, 1),
    point=(0, 0, -2),
    material=shiny_plane,
)
scene.add_surface(sphere_a)
scene.add_surface(sphere_b)
scene.add_surface(plane)
scene.add_surface(tri)

#==========  PRE-RENDER =============
pre = Scene()
pre.add_light(PointLight((0, 0, 0), BLACK))
pre.add_light(AmbientLight(BLACK))
pre.add_surface(Sphere((0, 0, 0), 1, blank))
pre.add_surface(Triangle((0, 0, 1), (0, 1, 0), (1, 0, 0), blank))
pre.add_surface(Plane((0, 0, 1), (0, 0, 0), blank))
pre_start = time.time()
pre.render()
pre_end = time.time()
print(f"pre-rendered scene in {pre_end - pre_start:.2f} seconds")

if PROFILE:
    with cProfile.Profile() as pr:
        image = scene.render(width=500, height=500)

    stats = pstats.Stats(pr)
    stats.strip_dirs().sort_stats("cumtime").print_stats(20)


#========== TESTING ==================
if TEST:
    times = list()

    #========= RENDER META ==============
    WIDTH = 1500
    HEIGHT = 1500
    BOUNCES = 2
    SAMPLES = 1

    #========= RENDER IMAGES ===========
    scene.max_bounces = BOUNCES
    for i in range(SAMPLES):
        render_start = time.time()
        image = scene.render(WIDTH, HEIGHT)
        render_end = time.time()
        t1 = (render_end - render_start)
        print(f"render {i + 1} took {t1:.2f} seconds.")
        times.append(t1)

    #========= PRINT IMAGE ===========
    Image.fromarray((np.clip(image, 0, 1) * 255).astype(np.uint8), 'RGB').save(f'test.png')

    script_end = time.time()
    avg_time = sum(times) / len(times)
    print(f"Average render time: {avg_time:.2f} seconds ({1 / avg_time:.2f} fps)")
    print(f"finished in {script_end - script_start:.2f} seconds")

#========= ANIMATION ============ 
if ANIMATE:
    from math import sin, cos, radians

    ani_start = time.time()
    ani = Scene(
        background_color=BLACK,
        max_bounces=2,
    )

    ball_x = Sphere(
        c=(5, 0, 0),
        r=1,
        material=white_glossy,
    )
    ball_y = Sphere(
        c=(0, 5, 0),
        r=1,
        material=white_glossy,
    )
    ball_z = Sphere(
        c=(-5, -5, 0),
        r=1,
        material=white_glossy,
    )
    plane_xy = Plane(
        normal = (0, 0, 1),
        point=(0, 0, -1),
        material=black_plane,
    )
    ani.add_surface(ball_x)
    ani.add_surface(ball_y)
    ani.add_surface(ball_z)
    ani.add_surface(plane)

    ambiance = AmbientLight(
        intensity=(0.5, 0.5, 0.5)
    )
    light_rx = PointLight(
        center=(16, 0, 5),
        intensity=(100, 0, 0),
    )
    light_gy = PointLight(
        center=(0, 16, 5),
        intensity=(0, 100, 0),
    )
    light_bz = PointLight(
        center=(-1.5, -1.5, 5),
        intensity=(0, 0, 100),
    )
    ani.add_light(ambiance)
    ani.add_light(light_rx)
    ani.add_light(light_gy)
    ani.add_light(light_bz)

    frame_times = list()
    FRAMES = 240
    for i in range(34, FRAMES):
        frame_start = time.time()
        deg = 360 / FRAMES
        sqrt_radius = 15
        x = cos(radians(i*deg))
        y = sin(radians(i*deg))
        eye = (sqrt_radius*x, sqrt_radius*y, 4)
        direction = (-x, -y, -0.5)

        cam = Camera(
            eye=eye,
            direction=direction,
            fov=64,
            aspect=1.0
        )
        ani.change_camera(cam)
        frame = ani.render(500, 500)
        Image.fromarray((np.clip(frame, 0, 1) * 255).astype(np.uint8), 'RGB').save(f'ani/frame{i+1}.png')

        frame_end = time.time()
        frame_times.append(frame_end - frame_start)
        print(f"Frame {i + 1}: {frame_end - frame_start:.2f} seconds")
    ani_end = time.time()
    print(f"{FRAMES} frames finished in: {ani_end - ani_start:.2f} seconds")
    print(f"Average frame time: {sum(frame_times) / FRAMES:.2f} seconds")

#ffmpeg -framerate 24 -i ani/frame%d.png ani/animation.mp4

#========= MATERIALS ===========
materials = {
    # --- Basic Matte Surfaces ---
    "red_matte": {
        "ka": (0.1, 0.0, 0.0),
        "kd": (0.8, 0.1, 0.1),
        "ks": (0.0, 0.0, 0.0),
        "shininess": 1
    },

    "green_matte": {
        "ka": (0.0, 0.1, 0.0),
        "kd": (0.1, 0.8, 0.1),
        "ks": (0.0, 0.0, 0.0),
        "shininess": 1
    },

    "blue_matte": {
        "ka": (0.0, 0.0, 0.1),
        "kd": (0.1, 0.1, 0.8),
        "ks": (0.0, 0.0, 0.0),
        "shininess": 1
    },

    # --- Glossy / Plastic ---
    "white_glossy": {
        "ka": (0.1, 0.1, 0.1),
        "kd": (0.8, 0.8, 0.8),
        "ks": (0.5, 0.5, 0.5),
        "shininess": 64
    },

    "yellow_plastic": {
        "ka": (0.1, 0.1, 0.0),
        "kd": (0.9, 0.9, 0.1),
        "ks": (0.4, 0.4, 0.2),
        "shininess": 32
    },

    # --- Metallic ---
    "silver_metal": {
        "ka": (0.05, 0.05, 0.05),
        "kd": (0.6, 0.6, 0.6),
        "ks": (0.9, 0.9, 0.9),
        "shininess": 128
    },

    "gold_metal": {
        "ka": (0.05, 0.04, 0.0),
        "kd": (0.83, 0.68, 0.21),
        "ks": (0.9, 0.8, 0.4),
        "shininess": 128
    },

    # --- Dark / Specular Mix ---
    "black_shiny": {
        "ka": (0.02, 0.02, 0.02),
        "kd": (0.05, 0.05, 0.05),
        "ks": (0.9, 0.9, 0.9),
        "shininess": 64
    }
}
