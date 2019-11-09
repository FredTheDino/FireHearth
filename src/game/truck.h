
struct Truck {
    Physics::Body body;
    Renderer::ParticleSystem boost_particles;
    Renderer::ParticleSystem smoke_particles;
    Vec2 dimension = V2(1, 1);
    Vec2 forward = V2(1, 0);
    f32 max_speed = 225.0;
    f32 boost_strength = 8.0;
    f32 rotation_speed = 5.0;

    void boost(f32 delta) {
        // TODO(ed): When you press the button, a huge fire burst should
        // happen and you should get slighly more speed.
        boost_particles.spawn();
        // boost_particles.spawn();
        body.velocity += forward * boost_strength * delta;
    }

    void update(f32 delta) {
        Physics::integrate(&body, delta);
        boost_particles.position = body.position;
        smoke_particles.position = body.position;
        body.velocity += V2(0, -delta * 5);

        {
            f32 velocity_angle = angle(forward);
            boost_particles.velocity_dir = {velocity_angle - 0.5,
                                            velocity_angle + 0.5};
            smoke_particles.velocity_dir = {velocity_angle - 1.0,
                                            velocity_angle + 1.0};
        }
        smoke_particles.spawn();
        if (down(Player::P1, Name::BOOST)) {
            boost(delta);
        }

        if (down(Player::P1, Name::UP)) {
            forward = rotate(forward, rotation_speed * delta);
        }
        if (down(Player::P1, Name::DOWN)) {
            forward = rotate(forward, -rotation_speed * delta);
        }

        // TODO(ed): Backheavy when falling.
        // pow(delta, 0.8)
        forward = normalize(forward + body.velocity * 0.1);
        Vec2 normal_dir = rotate_ccw(forward);
        f32 normal_speed  = dot(body.velocity, normal_dir);
        body.velocity -= normal_dir * CLAMP(-5, 5, normal_speed * pow(0.4, delta));
        if (length_squared(body.velocity) > max_speed) {
            body.velocity *= pow(0.2, delta);
        }
        smoke_particles.update(delta);
        boost_particles.update(delta);
    }

    void draw() {
        smoke_particles.draw();
        boost_particles.draw();
        Renderer::push_rectangle(body.position, dimension);
        Renderer::push_line(body.position, body.position + forward, V4(1, 0, 1, 1));
    }
};

Truck create_truck() {
    auto boost_particles = Renderer::create_particle_system(250, V2(0, 0));
    boost_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 1, 1, 1, 1);
    boost_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 4, 1, 1, 1);
    boost_particles.relative = false;
    boost_particles.one_color = true;
    boost_particles.one_alpha = true;
    boost_particles.velocity = {-2.0, -3.0};
    boost_particles.alive_time = {2, 3};
    boost_particles.rotation = {0, 0};
    boost_particles.spawn_size = {0.2, 0.4};
    boost_particles.die_size = {0.0, 0.0};

    auto smoke_particles = Renderer::create_particle_system(250, V2(0, 0));
    smoke_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 7, 1, 1, 1);
    smoke_particles.relative = false;
    smoke_particles.one_color = true;
    smoke_particles.one_alpha = true;
    smoke_particles.velocity = {-1.0, -2.0};
    smoke_particles.alive_time = {3, 4};
    smoke_particles.rotation = {0, 0};
    smoke_particles.spawn_size = {0.4, 0.6};
    smoke_particles.die_size = {0.0, 0.0};

    Truck truck = { Physics::create_body(square, 1.0, 0xFF, 0.0, 0.2),
                    boost_particles, smoke_particles};
    return truck;
}

