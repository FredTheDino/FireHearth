
void Bullet::update(f32 delta) {
    Physics::integrate(&body, delta);
}

void Bullet::draw() {
    Renderer::push_sprite(body.position, dimension, angle,
                          ASSET_PARTICLE_SPRITESHEEP,
                          V2(-BULLET_SPRITE.x, BULLET_SPRITE.y),
                          V2(BULLET_SPRITE.z + 1, BULLET_SPRITE.w + 1));
    bullet_trail.position = body.position;
    if (random_real() < 0.3)
        bullet_trail.spawn();
    // Physics::debug_draw_body(&body);
}

bool Bullet::is_dead() const {
    return hit_enemy || (Logic::now() - spawn_time) > BULLET_ALIVE_TIME;
}

void initalize_bullets() {
    bullets.reserve(128);

    bullet_trail = Renderer::create_particle_system(500, V2(0, 0));
    bullet_trail.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 1, 1, 1, 1);
    bullet_trail.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 4, 1, 1, 1);
    bullet_trail.relative = false;
    bullet_trail.one_color = true;
    bullet_trail.one_alpha = true;
    bullet_trail.velocity = {0.0, 0.0};
    bullet_trail.alive_time = {0.3, 1};
    bullet_trail.rotation = {0, 0};
    bullet_trail.spawn_size = {0.2, 0.4};
    bullet_trail.die_size = {0.0, 0.0};
}

void clear_bullets() {
    bullets.clear();
}

void create_bullet(Vec2 position, Vec2 forward) {
    f32 bulletSpread = random_real(-0.15, 0.15);
    forward = rotate(forward, bulletSpread);
    Bullet bullet;
    bullet.body = Physics::create_body(square, 1.0, 0xFF, 0.0, 0.0);
    bullet.body.scale = bullet.dimension;
    bullet.spawn_time = Logic::now();
    bullet.body.position = position;
    bullet.angle = -angle(forward);
    bullet.body.velocity = forward * bullet.speed;
    bullet.body.rotation = bullet.angle;
    bullets.push_back(bullet);
}

void update_bullets(f32 delta) {
    std::vector<u32> to_remove;
    for (Bullet &bullet : bullets) {
        bullet.update(delta);
    }
    auto is_dead = [](const Bullet &bullet) {
        return bullet.is_dead();
    };
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), is_dead),
        bullets.end());
    bullet_trail.update(delta);
}

void draw_bullets() {
    for (Bullet &bullet : bullets)
        bullet.draw();
    bullet_trail.draw();
}

void Truck::boost(f32 delta) {
    // TODO(ed): When you press the button, a huge fire burst should
    // happen and you should get slighly more speed.
    boost_particles.spawn();
    // boost_particles.spawn();
    body.velocity += forward * TRUCK_BOOST_STRENGTH * delta;

    if (boost_timer > 0.0 && !max_out) {
        if (random_real() < 0.5)
            super_particles.spawn();
        body.velocity += normalize(body.velocity) * LERP(TRUCK_BOOST_MAX, boost_timer, 0.0) * delta;
    }
}

void Truck::update(f32 delta) {
    Physics::integrate(&body, delta);
    super_particles.position = body.position - forward * dimension.x * 0.5;
    boost_particles.position = body.position - forward * dimension.x * 0.5;
    smoke_particles.position = body.position - forward * dimension.x * 0.5;
    body.velocity += V2(0, -delta * 5);

    {
        f32 velocity_angle = -angle(forward);
        super_particles.velocity_dir = {velocity_angle - 3.0f,
            velocity_angle + 3.0f};
        boost_particles.velocity_dir = {velocity_angle - 0.5f,
            velocity_angle + 0.5f};
        smoke_particles.velocity_dir = {velocity_angle - 1.0f,
            velocity_angle + 1.0f};
    }
    smoke_particles.spawn();

    if (body.position.y <= groundLevel) {
	    game_over = true;
    }

    if (body.position.y >= WORLD_TOP_EDGE * 1.2) {
        body.velocity += V2(0, -WIND_FORCE) * ABS(WORLD_TOP_EDGE * 2 - body.position.y);
    }

    if (body.position.x >= WORLD_RIGHT_EDGE) {
        body.velocity += V2(-WIND_FORCE, 0) * ABS(WORLD_RIGHT_EDGE - body.position.x);
    }

    if (body.position.x <= WORLD_LEFT_EDGE) {
        body.velocity += V2(WIND_FORCE, 0) * ABS(WORLD_LEFT_EDGE - body.position.x);
    }

    if (pressed(Player::P1, Name::BOOST)) {
        body.velocity += normalize(body.velocity) * TRUCK_BOOST_INITIAL;
    }

    if (down(Player::P1, Name::BOOST)) {
        boost(delta);  
        boost_timer -= delta;
    } else {
        boost_timer += delta * 0.4;
    }

    if (boost_timer < 0.0)
        max_out = true;
    if (max_out && boost_timer > TRUCK_BOOST_TIME_MAX)
        max_out = false;

    boost_timer = CLAMP(TRUCK_BOOST_TIME_MIN, TRUCK_BOOST_TIME_MAX, boost_timer);

    if (down(Player::P1, Name::UP))
        forward = rotate(forward, TRUCK_ROTATION_SPEED * delta);
    if (down(Player::P1, Name::DOWN))
        forward = rotate(forward, -TRUCK_ROTATION_SPEED * delta);
    
    // TODO(ed): Backheavy when falling.
    forward = normalize(forward + body.velocity * TRUCK_VELOCITY_WEIGHT);
    Vec2 normal_dir = rotate_ccw(forward);
    f32 normal_speed  = dot(body.velocity, normal_dir);
    f32 normal_slowdown = normal_speed * pow(TRUCK_VELOCITY_DAMPING, delta);
    f32 velocity_squared = CLAMP(0.0, 1.0, length_squared(body.velocity) / TRUCK_FLIGHT_CONSTANT);
    boost_to_kill = (velocity_squared > 0.10) && down(Player::P1, Name::BOOST);

    body.velocity -= normal_dir * CLAMP(-5, 5, normal_slowdown) * velocity_squared;
    if (length_squared(body.velocity) > TRUCK_MAX_SPEED)
        body.velocity *= pow(0.2, delta);

    if (down(Player::P1, Name::SHOOT) &&
        Logic::now() >= last_shot + TRUCK_SHOOT_DELAY) {
            create_bullet(body.position + forward * dimension.x * 0.4, forward);
            last_shot = Logic::now();
	    Mixer::play_sound(ASSET_NOISE, 1.0, 0.5);

    }

    super_particles.update(delta);
    smoke_particles.update(delta);
    boost_particles.update(delta);
}

void Truck::draw() {
    smoke_particles.draw();
    boost_particles.draw();
    super_particles.draw();
    Renderer::push_sprite(body.position, dimension, -angle(-forward),
            ASSET_PARTICLE_SPRITESHEEP,
            V2(TRUCK_SPRITE.x, TRUCK_SPRITE.y),
            V2(TRUCK_SPRITE.z, TRUCK_SPRITE.w));
    // Physics::debug_draw_body(&body);
}

void Truck::super_boost() {
    boost_timer = TRUCK_BOOST_TIME_MAX;
    max_out = false;
}

void Truck::reset() {
    body.position = V2(0, WORLD_TOP_EDGE + 3);
    body.velocity = V2(0, -5);
    forward = V2(0, -1);
    last_shot = 0;
    boost_timer = TRUCK_BOOST_TIME_MAX;
    max_out = false;

    boost_particles.clear();
    smoke_particles.clear();
    super_particles.clear();
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

    auto super_particles = Renderer::create_particle_system(250, V2(0, 0));
    super_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 22, 1, 1, 1);
    super_particles.relative = false;
    super_particles.one_color = true;
    super_particles.one_alpha = true;
    super_particles.velocity = {-2.0, -3.0};
    super_particles.alive_time = {5, 6};
    super_particles.damping = {0.8, 0.9};
    super_particles.rotation = {0, 0};
    super_particles.spawn_size = {0.6, 1.0};
    super_particles.die_size = {0.0, 0.0};

    Truck truck = { Physics::create_body(square, 1.0, 0xFF, 0.0, 0.2),
                    super_particles, boost_particles, smoke_particles};
    truck.reset();
    return truck;
}

