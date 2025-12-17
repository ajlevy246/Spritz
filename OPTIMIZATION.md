# Inverse Graphics

The goal of this project was to use this simple raytracer for some inverse graphics problems, where we try to infer 3d parameters given 2d images. The examples run demonstrate how we can match target images by optimizing scene properties like lighting, materials, and geometry.

### Key Features:
* Photometric loss-based optimization
* Finite difference gradient estimation
* ADAM optimizer for fast convergence
* Lighting and Material parameter recovery
* Mesh-based scene geometry

## Methodology

First, we set the problem up as an optimization problem in the following manner:
1. **Loss Function**: The L2 Photometric loss is used as a measure of the 'distance' between two rendered images. It's implementation is below, computing the mean squared error between corresponding pixels.
```cpp
double photo_loss(const std::vector<Vec3> &im1, const std::vector<Vec3> &im2) {
    int n = im1.size();
    assert(n == im2.size());
    double diff = 0.0;
    for (int i = 0; i < n; i++) {
        diff += (im1.at(i) - im2.at(i)).length_sq();
    }
    return diff / n;
}
```

2. **Gradient Estimates**: While more advanced differentiable raytracing systems use a combination of auto-diff, analytical integration, and more advanced methods, for these examples I just used a simple finite difference method to estimate gradients. For more advanced problems, a stochastic-based approach to gradient estimates should be used instead. 

    For example, we may estimate the gradient of the x-coordinate of a single light in the scene (`sun`) as:


```cpp
sun.position.x += pertubation;
        high_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        sun.position.x -= pertubation2;
        low_loss = photo_loss(ground_truth, scene.render(WIDTH, HEIGHT));
        grad.x = (high_loss - low_loss) / pertubation2;
```

3. **Algorithm**: I used the ADAM optimization algorithm for these examples for it's adaptive learning rates and momentum for faster convergence and to smooth over small minima. 

<img width="770" height="253" alt="visualization" src="https://github.com/user-attachments/assets/1f7268e0-9f2d-421d-915d-fddd415950b2" />

>> A visualization of the photometric loss between two renders.

## Examples 

I've set up some simple examples in the following folders to get a sense of what these problems look like and how difficult they are. These three examples get progressively more complex and demonstrate the system's capabilities and limitations. 

To build these examples, run `make examples` from the root directory. Then navigate to an example's folder, and run the compiled `./example` binary.


### 1. Light Parameter Recovery - `Well_Posed_Example/`
This example is a proof-of-concept problem that uses a simple render to recover light position and intensity:

#### Ground Truth:
<img width="500" height="500" alt="ground_truth" src="https://github.com/user-attachments/assets/17e8b875-48bd-40a6-b70c-bef10726cae0" />

#### Initial Conditions:
<img width="500" height="500" alt="initial_guess" src="https://github.com/user-attachments/assets/a2d0def5-251e-4a31-8aac-d8263e10f533" />

#### Optimization Progress:

https://github.com/user-attachments/assets/af463a68-80ee-48ed-9838-66c4e520447d

>> We see that the algorithm successfully recovers the original light parameters on this well-posed problem.

### 2. Material Parameters Recovery - `Ill_Posed_Example/`
This example is a more challenging problem recovering material properties (ambient, diffuse, and specular coefficients) of a reflective sphere. Since pertubations of these parameters don't effect the entire render and have less impact on individual pixels, the loss function is a less accurate representation of the difference between pertubations.

#### Ground Truth:
<img width="500" height="500" alt="Ill Posed - Ground Truth" src="https://github.com/user-attachments/assets/8c11436b-09be-4222-a1ae-ce75f219d77d" />

#### Why Ill-Posed?
This example presents two key challenges:

1. The sphere occupies a small fraction of the image, while the loss considers all pixels.
2. L2 is less sensitive to outlier, so we may expect the optimizer to increase ambient coefficients (brightening the entire sphere) rather than recovering the specular highlights that are more subtle.

#### Initial Guess:

<img width="500" height="500" alt="Ill Posed - Initial Conditions" src="https://github.com/user-attachments/assets/877a8ee6-7919-4916-b4b2-ac3181bdeba9" />

>> Note the small differences in the reflective sphere's appearance.

#### Optimization Progress:
Finally, we apply ADAM. Despite the challenges, the algorithm succesfully recovers specular highlights with minimal hyperparameter tuning. The final ambient coefficient is a bit higher than the true parameters, but overall this performed better than expected.

https://github.com/user-attachments/assets/bd31dcca-154a-4ca5-81ff-12d296ef44f5

### 3. Real Image Reconstruction - `Inverse_Problem_Example`
This example is an attempt at a much more complex problem, where we attempt to recover scene parameters from a real photograph rather than a synthetic render.

#### Target Image (Screenshot from the Web):

<img width="250" height="248" alt="snowden" src="https://github.com/user-attachments/assets/0b393bd8-9c42-4a03-aeed-eba7ff66a64d" />

#### Initial Scene Setup:

<img width="250" height="250" alt="Inverse Problem - Initial Guess" src="https://github.com/user-attachments/assets/01814112-4eee-49e2-be5c-0d875a1da7a6" />

**Goal:** Recover material properties and lighting configuration that produced the original photograph.

**Challenges**: Many...
 - For one, the shading model used in the target image is fundamentally different than the one used in the Spritz raytracer (smooth shading vs. Lambertian materials w/ Shlick approximation). From the start, this means we can't expect to fully reconstruct the target.
 - The camera parameters and lens effects are unknown. The initial scene setup above shows my best guess, but we see that the geometry does not line up perfectly. This already makes the problem extremely difficult, as photometric loss will completely miss the specular highlights on the statue's brow and collar and instead seek an overall brighter image.

 #### Optimization Results:

 

## Running The Project

### 1. Download the Project Files:

Work in progress...