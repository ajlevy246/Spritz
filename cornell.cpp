
    // --- Build cornell box ---
    Scene scene;
    scene.background = Vec3(0.0, 0.0, 0.0);
    scene.max_bounces = 1;

    // --- Create camera ---
    Vec3 eye(0, 25, 2);
    Vec3 lookat(0, -1, 1);
    double aspect = 1;
    double fov = 114.0;
    scene.set_camera(std::make_shared<PerspectiveCam>(eye, lookat, aspect, fov));
    // scene.set_camera(std::make_shared<OrthographicCam>(eye, lookat, aspect));

    // --- Walls ---
    Plane right = Plane( // Red
        Vec3(2, 0, 0),
        Vec3(-1, 0, 0),
        Material(
            Vec3(0.45, 0.1, 0.1),
            Vec3(0.35, 0.1, 0.1),
            Vec3(0, 0, 0),
            0
        )
    );
    Plane left = Plane( // Blue
        Vec3(-2, 0, 0),
        Vec3(1, 0, 0),
        Material(
            Vec3(0.1, 0.1, 0.45),
            Vec3(0.1, 0.1, 0.35),
            Vec3(0, 0, 0),
            0
        )
    );
    Plane back = Plane( // White
        Vec3(0, 1, 0),
        Vec3(0, -1, 0),
        Material(
            Vec3(0.1, 0.1, 0.1),
            Vec3(0.5, 0.5, 0.5),
            Vec3(0, 0, 0),
            0
        )
    );
    Plane floor = Plane( // White
        Vec3(0, 0, 1),
        Vec3(0, 0, 0),
        Material(
            Vec3(0.1, 0.1, 0.1),
            Vec3(0.5, 0.5, 0.5),
            Vec3(0, 0, 0),
            0
        )
    );
    Plane ceiling = Plane( // White
        Vec3(0, 0.1, -1),
        Vec3(0, 0, 2.25),
        Material(
            Vec3(0.2, 0.2, 0.2),
            Vec3(0.4, 0.4, 0.4),
            Vec3(0, 0, 0),
            0
        )
    );

    // --- Inner Objects ---
    Sphere sphere = Sphere( // Gold Metal
        0.5,
        Vec3(0, 2, 0.5),
        gold_metal
    );

    // --- Light Sources ---
    PointLight lightA = PointLight(
        Vec3(0, 1.5, 1.95),
        Vec3(5, 5, 5)
    );
    AmbientLight ambient = AmbientLight(
        Vec3(0.5, 0.5, 0.5)
    );

    // --- Add Objects ---
    scene.add_surface(&left);
    scene.add_surface(&right);
    scene.add_surface(&back);
    scene.add_surface(&floor);
    scene.add_surface(&ceiling);

    scene.add_surface(&sphere);

    // --- Add lights ---
    scene.add_light(&lightA);
    scene.add_light(&ambient);