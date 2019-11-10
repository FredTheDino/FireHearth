const u32 TRASHBAG_INDEX = 0;
const f32 TRASH_SIZE = 5;
const u32 TRASH_HP = 2;
const u32 BANANA_INDEX = 1;
const f32 BANANA_SIZE = 5;
const u32 BANANA_HP = 5;
const u32 BIGBOI_INDEX = 2;
const f32 BIGBOI_SIZE = 10;
const u32 BIGBOI_HP = 10;
const f32 BIGBOI_BULLET_SIZE = 5;
const f32 BIGBOI_BULLET_SPEED = 40;
const f32 BIGBOI_BULLET_ACCELERATION = -15;

struct Enemy : public Entity {
    Enemy(Vec2 pos, Vec2 dim, f32 rotation, u32 hp) :
        Entity(pos, dim, ASSET_TEST, rotation),
        hp(hp),
        time(0),
        animation_delay(1) {}

    virtual ~Enemy() {}

    virtual bool boost_killable() {
        return true;
    }

    u32 hp;
    f32 time;
    f32 animation_delay;
    std::vector<AssetID> images;

    void animate(f32 time) {
        image = images[(u32)(time / animation_delay) % images.size()];
    }

    bool is_dead() {
        return hp <= 0;
    }

};

struct TrashBag : public Enemy{
    TrashBag(Vec2 pos) :
    Enemy(pos, V2(TRASH_SIZE, TRASH_SIZE), 0, TRASH_HP),
    velocity(V2(0,0)),
    orig_pos(pos) {
        animation_delay = 0.5;
        images.push_back(ASSET_TRASH_SLEEP);
    }

    const f32 SPEED = 6;
    f32 buryTime = 0;
    bool onGround = false;

    void update(f32 delta) {
        time += delta;
        animate(time);

        if (pos.y <= groundLevel) {
            buryTime += delta;
            onGround = true;
        }

        if (!onGround) {
            velocity.y = -SPEED;
            rotation = sin(time) / 3;
        } else {
            velocity.y = 0;
            animation_delay = 0.1;
            images.pop_back();
            images.push_back(ASSET_TRASH);
            images.push_back(ASSET_TRASH_WALK);
            rotation = sin(time * 10) / 5;

            if (buryTime >= 3) {
                goalTrashLevel++;
                groundLevel++;
                hp = 0;
            }
        }
        pos += velocity * delta;
    }

    Vec2 velocity;
    Vec2 orig_pos;
};

struct Banana : public Enemy {
    const f32 SPEED = 5;
    const f32 SPEED_CHASING = 10;
    const f32 CHASE_DIST = 30;

    Banana(Vec2 pos) :
        Enemy(pos, V2(BANANA_SIZE, BANANA_SIZE), 0, BANANA_HP),
        velocity(V2(SPEED, 0)),
        orig_pos(pos) {
            images.push_back(ASSET_BANANA1);
            images.push_back(ASSET_BANANA2);
            images.push_back(ASSET_BANANA3);
            images.push_back(ASSET_BANANA2);
    }

    void update(f32 delta) override {
        time += delta;
        animate(time);

        if (pos.x <= WORLD_LEFT_EDGE - 10) {
            velocity.x = SPEED;
        } else if (pos.x >= WORLD_RIGHT_EDGE - 10) {
            velocity.x = -SPEED;
        }

        Vec2 to_player = get_truck_pos() - pos;

        if (length(to_player) < CHASE_DIST) {
            Vec2 goal = normalize(to_player) * SPEED_CHASING;
            velocity = LERP(velocity, 0.2, goal);
            animation_delay = 0.25;
        } else {
            Vec2 goal = V2(velocity.x < 0 ? -SPEED : SPEED, sin(3 * time) * 5);
            velocity = LERP(velocity, 0.2, goal);
            animation_delay = 0.5;
        }

        pos += velocity * delta;
    }

    Vec2 velocity;
    Vec2 orig_pos;
};

struct Spawner;

struct BigBoi : public Enemy {
    const f32 SPEED = 5;
    const f32 SPEED_CHASING = 10;

    BigBoi(Vec2 pos, Spawner* spawner) :
        Enemy(pos, V2(BIGBOI_SIZE, BIGBOI_SIZE), 0, BIGBOI_HP),
        velocity(V2(0, 0)),
        orig_pos(pos),
        spawner(spawner),
        fired(false) {
            animation_delay = 0.25;
            images.push_back(ASSET_BIGBOI_LEFT);
            images.push_back(ASSET_BIGBOI_RIGHT);
    }

    virtual bool boost_killable() {
        return false;
    }

    void update(f32 delta) override;

    Vec2 velocity;
    Vec2 orig_pos;
    Spawner* spawner;
    bool fired;
};

struct BigBoiBullet : public Enemy {
    BigBoiBullet(Vec2 pos, f32 x_velocity) :
        Enemy(pos, V2(BIGBOI_BULLET_SIZE, BIGBOI_BULLET_SIZE), 0, 1),
        velocity(V2(x_velocity, BIGBOI_BULLET_SPEED)),
        acceleration(V2(0, BIGBOI_BULLET_ACCELERATION)),
        orig_y(pos.y) {
            image = ASSET_BIGBOI_BULLET;
    }

    void update(f32 delta) override {
        velocity += acceleration * delta;
        pos += velocity * delta;
        if (pos.y < orig_y) hp = 0;
    }

    Vec2 velocity;
    Vec2 acceleration;
    f32 orig_y;
};

struct Spawner {
    Spawner(std::vector<Enemy*>* enemies) :
        enemies(enemies),
        time(0),
        threat(0) {}

    void update(f32 delta) {
        time += delta;

        if (time < 20) {
            threat = 0;
        } else if (time < 40) {
            threat = 1;
        } else if (time < 80) {
            threat = 2;
        } else if (time < 120) {
            threat = 3;
        }

        // Spawn according to threat level
        switch (threat) {
            case 0:
                if (time - last_spawn[TRASHBAG_INDEX] > 1.5) {
                    spawn_trashbag();
                    last_spawn[TRASHBAG_INDEX] = time;
                }
                break;
            case 1:
                if (time - last_spawn[TRASHBAG_INDEX] > 3) {
                    spawn_trashbag();
                    last_spawn[TRASHBAG_INDEX] = time;
                }
                if (time - last_spawn[BANANA_INDEX] > 5) {
                    spawn_banana();
                    last_spawn[BANANA_INDEX] = time;
                }
                break;
            case 2:
                if (time - last_spawn[TRASHBAG_INDEX] > 2) {
                    spawn_trashbag();
                    last_spawn[TRASHBAG_INDEX] = time;
                }
                if (time - last_spawn[BANANA_INDEX] > 7) {
                    spawn_banana();
                    last_spawn[BANANA_INDEX] = time;
                }
                if (time - last_spawn[BIGBOI_INDEX] > 25) {
                    spawn_bigboi();
                    last_spawn[BIGBOI_INDEX] = time;
                }
                break;
            case 3:
                LOG("%f", threat3_coeff(time));
                if (time - last_spawn[TRASHBAG_INDEX] > 2 * threat3_coeff(time)) {
                    spawn_trashbag();
                    last_spawn[TRASHBAG_INDEX] = time;
                }
                if (time - last_spawn[BANANA_INDEX] > 8 * threat3_coeff(time)) {
                    spawn_banana();
                    last_spawn[BANANA_INDEX] = time;
                }
                if (time - last_spawn[BIGBOI_INDEX] > 25 * threat3_coeff(time)) {
                    spawn_bigboi();
                    last_spawn[BIGBOI_INDEX] = time;
                }
                break;
            default:
                break;
        }
    }

    void spawn_banana() {
        f32 x = random_real() < 0.5 ? WORLD_LEFT_EDGE - 5 : WORLD_RIGHT_EDGE + 5;
        f32 y = random_real(WORLD_TOP_EDGE - 5, WORLD_BOTTOM_EDGE + 5);
        enemies->push_back(new Banana(V2(x, y)));
    }

    void spawn_trashbag() {
        f32 x = random_real(WORLD_LEFT_EDGE * 0.9, WORLD_RIGHT_EDGE * 0.9);
        f32 y = WORLD_TOP_EDGE + 5;
        enemies->push_back(new TrashBag(V2(x, y)));
    }

    void spawn_bigboi() {
        f32 x = random_real(WORLD_LEFT_EDGE * 0.9, WORLD_RIGHT_EDGE * 0.9);
        f32 y = WORLD_BOTTOM_EDGE - 5;
        enemies->push_back(new BigBoi(V2(x,y), this));
    }

    void spawn_bigboibullets(Vec2 pos) {
        enemies->push_back(new BigBoiBullet(pos, 3));
        enemies->push_back(new BigBoiBullet(pos, 0));
        enemies->push_back(new BigBoiBullet(pos, -3));
    }

    void reset() {
        time = 0;
        last_spawn[0] = -1;
        last_spawn[1] = -1;
        last_spawn[2] = -1;
    }

    f32 threat3_coeff(f32 time) {
        return MAX(0.5, pow(0.66, (time - 120) / 40));
    }

    std::vector<Enemy*>* enemies;
    f32 time;
    u32 threat;
    f32 last_spawn[3] = { -1, -1, -1 };
};

Renderer::ParticleSystem hit_particles;
std::vector<Enemy*> enemies;
Spawner spawner(&enemies);

void initalize_enemies() {
    spawner.reset();
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    enemies.reserve(64);

    hit_particles = Renderer::create_particle_system(500, V2(0, 0));
    hit_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 19, 1, 1, 1);
    hit_particles.add_sprite(ASSET_PARTICLE_SPRITESHEEP, 22, 1, 1, 1);
    hit_particles.one_alpha = true;
    hit_particles.alive_time = {0.5, 1.0};
    hit_particles.rotation = {};
    hit_particles.velocity_dir = {-PI / 2, PI / 2};
    hit_particles.velocity = {-5, 5};
    hit_particles.damping = {0.8, 1.0};
    hit_particles.acceleration = {2.0, 3.0};
    hit_particles.spawn_size = {1.0, 0.9};
    hit_particles.die_size = {0.0, 0.0};
}

void emit_hit_particles(Vec2 position) {
    hit_particles.position = position;
    u32 count = random_int() % 10 + 15;
    for (u32 i = 0; i < count; i++) {
        hit_particles.spawn();
    }
}

void emit_dead_particles(Vec2 position) {
    hit_particles.position = position;
    auto old = hit_particles.spawn_size;
    hit_particles.spawn_size = {2.0, 2.5};
    u32 count = random_int() % 10 + 15;
    for (u32 i = 0; i < count; i++) {
        hit_particles.spawn();
    }
    hit_particles.spawn_size = old;
}

void emit_boost_to_kill_particles(Vec2 position) {
    hit_particles.position = position;
    auto old = hit_particles.velocity;
    hit_particles.velocity = {5.0, 10.0};
    u32 count = random_int() % 10 + 15;
    for (u32 i = 0; i < count; i++) {
        hit_particles.spawn();
    }
    hit_particles.velocity = old;
}

void update_enemies(f32 delta) {
    hit_particles.update(delta);
    for (s32 i = enemies.size() - 1; i >= 0; i--) {
        enemies[i]->update(delta);
        if (enemies[i]->is_dead()) {
            score_kill_enemy();
            emit_dead_particles(enemies[i]->pos);
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        }
    }
}

void draw_entity(Entity* entity) {
    if (entity->image != NO_ASSET) {
        Image* img = Asset::fetch_image(entity->image);
        Renderer::push_sprite(entity->pos,
                -entity->dim,
                entity->rotation,
                entity->image,
                V2(0,0),
                V2(img->width, img->height));
    } else {
        Renderer::push_rectangle(entity->pos, entity->dim);
    }
}

void draw_enemies() {
    hit_particles.draw();
    for (Enemy* enemy : enemies) {
        draw_entity(enemy);
        Physics::Body body = enemy->get_body();
        //Physics::debug_draw_body(&body);
    }
}


void BigBoi::update(f32 delta) {
    time += delta;

    if (time < 4) {
        animate(time);
        Vec2 to_player = get_truck_pos() - pos;
        to_player.y = 0;

        Vec2 goal = normalize(to_player) * SPEED_CHASING;
        velocity = LERP(velocity, 0.2, goal);

        pos += velocity * delta;
    } else if (time < 4.5) {
        image = ASSET_BIGBOI_PREP_FIRE;
    } else if (time < 4.75) {
        image = ASSET_BIGBOI_FIRE;
        if (!fired) {
            fired = true;
            spawner->spawn_bigboibullets(pos);
        }
    } else {
        fired = false;
        time = 0;
    }

    pos.y += (groundLevel - pos.y) * 0.2 * delta;
}
