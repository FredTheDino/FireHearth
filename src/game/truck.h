
Vec4 truck_sprite = V4(1, 4, 15, 9);
Vec4 bullet_sprite = V4(10, 0, 6, 3);
const f32 BULLET_ALIVE_TIME = 5.0;

struct Bullet {
    Physics::Body body;
    f32 spawn_time;
    f32 angle;
    f32 speed = 20.0;
    Vec2 dimension = V2(6, 2) / 3.0;

    void update(f32 delta) {
        Physics::integrate(&body, delta);
    }

    void draw() {
        Renderer::push_sprite(body.position, dimension, angle,
                              ASSET_PARTICLE_SPRITESHEEP,
                              V2(-bullet_sprite.x, bullet_sprite.y),
                              V2(bullet_sprite.z + 1, bullet_sprite.w + 1));
        // Physics::debug_draw_body(&body);
    }
};

std::vector<Bullet> bullets;

void initalize_bullets() {
    bullets.reserve(128);
}

void create_bullet(Vec2 position, Vec2 forward) {
    Bullet bullet;
    bullet.body = Physics::create_body(square, 1.0, 0xFF, 0.0, 0.0);
    bullet.body.scale = bullet.dimension;
    bullet.spawn_time = Logic::now();
    bullet.body.position = position;
    bullet.body.velocity = forward * bullet.speed;
    bullet.angle = -angle(forward);
    bullet.body.rotation = bullet.angle;
    bullets.push_back(bullet);
}

void update_bullets(f32 delta) {
    std::vector<u32> to_remove;
    for (Bullet &bullet : bullets) {
        bullet.update(delta);
    }
    auto is_dead = [](const Bullet &bullet) {
        return (Logic::now() - bullet.spawn_time) > BULLET_ALIVE_TIME;
    };
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), is_dead),                
        bullets.end());
}

void draw_bullets() {
    for (Bullet &bullet : bullets)
        bullet.draw();
}

struct Truck {
    Physics::Body body;
    Renderer::ParticleSystem boost_particles;
    Renderer::ParticleSystem smoke_particles;
    Vec2 dimension = V2(15, 9) / 3.0;
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
        boost_particles.position = body.position - forward * dimension.x * 0.5;
        smoke_particles.position = body.position - forward * dimension.x * 0.5;
        body.velocity += V2(0, -delta * 5);

        {
            f32 velocity_angle = -angle(forward);
            boost_particles.velocity_dir = {velocity_angle - 0.5f,
                                            velocity_angle + 0.5f};
            smoke_particles.velocity_dir = {velocity_angle - 1.0f,
                                            velocity_angle + 1.0f};
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
        forward = normalize(forward + body.velocity * 0.1);
        Vec2 normal_dir = rotate_ccw(forward);
        f32 normal_speed  = dot(body.velocity, normal_dir);
        body.velocity -= normal_dir * CLAMP(-5, 5, normal_speed * pow(0.4, delta));
        if (length_squared(body.velocity) > max_speed) {
            body.velocity *= pow(0.2, delta);
        }

        if (pressed(Player::P1, Name::SHOOT))
            create_bullet(body.position + forward * dimension.x * 0.5,
                          forward);

        smoke_particles.update(delta);
        boost_particles.update(delta);
    }

    void draw() {
        smoke_particles.draw();
        boost_particles.draw();
        Renderer::push_sprite(body.position, dimension, -angle(-forward),
                              ASSET_PARTICLE_SPRITESHEEP,
                              V2(truck_sprite.x, truck_sprite.y),
                              V2(truck_sprite.z, truck_sprite.w));
        // Physics::debug_draw_body(&body);
    }
};

Truck create_truck() {
    auto boost_particles = Renderer::create_particle_system(250, V2(0, 0));
    boost_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 1, 1, 1, 1);
    boost_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 4, 1, 1, 1);
    boost_particles.relative = false;
    boost_particles.one_color = true;
    boost_particles.one_alpha = true;
    boost_particles.velocity = {-6.0, -8.0};
    boost_particles.alive_time = {0.5, 1};
    boost_particles.rotation = {0, 0};
    boost_particles.spawn_size = {0.5, 0.6};
    boost_particles.die_size = {0.0, 0.0};

    auto smoke_particles = Renderer::create_particle_system(250, V2(0, 0));
    smoke_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 7, 1, 1, 1);
    smoke_particles.relative = false;
    smoke_particles.one_color = true;
    smoke_particles.one_alpha = true;
    smoke_particles.velocity = {-1.0, -2.0};
    smoke_particles.alive_time = {3, 4};
    smoke_particles.rotation = {0, 0};
    smoke_particles.spawn_size = {0.6, 1.0};
    smoke_particles.die_size = {0.0, 0.0};

    Truck truck = { Physics::create_body(square, 1.0, 0xFF, 0.0, 0.2),
                    boost_particles, smoke_particles};
    return truck;
}

