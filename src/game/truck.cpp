
void Bullet::update(f32 delta) {
    Physics::integrate(&body, delta);
}

void Bullet::draw() {
    Renderer::push_sprite(body.position, dimension, angle,
                          ASSET_PARTICLE_SPRITESHEEP,
                          V2(-BULLET_SPRITE.x, BULLET_SPRITE.y),
                          V2(BULLET_SPRITE.z + 1, BULLET_SPRITE.w + 1));
    // Physics::debug_draw_body(&body);
}

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

void Truck::boost(f32 delta) {
    // TODO(ed): When you press the button, a huge fire burst should
    // happen and you should get slighly more speed.
    boost_particles.spawn();
    // boost_particles.spawn();
    body.velocity += forward * TRUCK_BOOST_STRENGTH * delta;
    if (pressed(Player::P1, Name::BOOST)) {
        body.velocity += forward * TRUCK_BOOST_STRENGTH * delta;
    }
}

void Truck::update(f32 delta) {
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
    if (down(Player::P1, Name::BOOST))
        boost(delta);

    if (down(Player::P1, Name::UP))
        forward = rotate(forward, TRUCK_ROTATION_SPEED * delta);
    if (down(Player::P1, Name::DOWN))
        forward = rotate(forward, -TRUCK_ROTATION_SPEED * delta);

    // TODO(ed): Backheavy when falling.
    forward = normalize(forward + body.velocity * TRUCK_VELOCITY_WEIGHT);
    Vec2 normal_dir = rotate_ccw(forward);
    f32 normal_speed  = dot(body.velocity, normal_dir);
    f32 normal_slowdown = normal_speed * pow(TRUCK_VELOCITY_DAMPING, delta);
    body.velocity -= normal_dir * CLAMP(-5, 5, normal_slowdown);
    if (length_squared(body.velocity) > TRUCK_MAX_SPEED)
        body.velocity *= pow(0.2, delta);

    if (pressed(Player::P1, Name::SHOOT))
        create_bullet(body.position + forward * dimension.x * 0.5, forward);

    smoke_particles.update(delta);
    boost_particles.update(delta);
}

void Truck::draw() {
    smoke_particles.draw();
    boost_particles.draw();
    Renderer::push_sprite(body.position, dimension, -angle(-forward),
            ASSET_PARTICLE_SPRITESHEEP,
            V2(TRUCK_SPRITE.x, TRUCK_SPRITE.y),
            V2(TRUCK_SPRITE.z, TRUCK_SPRITE.w));
    // Physics::debug_draw_body(&body);
}

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


