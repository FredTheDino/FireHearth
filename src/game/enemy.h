struct Enemy : public Entity {
    Enemy(Vec2 pos, Vec2 dim, f32 rotation, u32 hp) :
        Entity(pos, dim, ASSET_TEST, rotation),
        hp(hp),
        time(0),
        animation_delay(1) {}

    virtual ~Enemy() {}

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

struct Banana : public Enemy {
    Banana(Vec2 pos) :
        Enemy(pos, V2(5, 5), 0, 10),
        velocity(V2(0, 0)),
        orig_pos(pos) {
            animation_delay = 0.5;
            images.push_back(ASSET_BANANA1);
            images.push_back(ASSET_BANANA2);
            images.push_back(ASSET_BANANA3);
            images.push_back(ASSET_BANANA2);
    }

    const f32 SPEED = 5;
    const f32 SPEED_CHASING = 7;
    const f32 CHASE_DIST = 30;

    void update(f32 delta) override {
        time += delta;
        animate(time);

        if (pos.x <= WORLD_LEFT_EDGE) {
            velocity.x = SPEED;
        } else if (pos.x >= WORLD_RIGHT_EDGE) {
            velocity.x = -SPEED;
        }
        velocity.y = sin(time) / 3;

        Vec2 to_player = get_truck_pos() - pos;

        if (length(to_player) < CHASE_DIST) {
            pos += normalize(to_player) * SPEED_CHASING * delta;
        } else {
            pos += velocity * delta;
        }
    }

    Vec2 velocity;
    Vec2 orig_pos;
};

struct TrashBag : public Enemy{
    TrashBag(Vec2 pos) :
	Enemy(pos, V2(5,5), 0, 1),
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
	
	if(pos.y <= groundLevel){
            buryTime += delta ;
	    onGround = true;
	}
	
	
	if(!onGround){
	    velocity.y = -SPEED;
	    rotation = sin(time) / 3;
	}else{
	    velocity.y =0;
	    animation_delay = 0.1;
	    images.pop_back();
	    images.push_back(ASSET_TRASH);
            images.push_back(ASSET_TRASH_WALK);
	    rotation = sin(time*10) /5;
		
	    if (buryTime >= 3){
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

struct Spawner {
    Spawner(std::vector<Enemy*>* enemies) :
        enemies(enemies),
        threat(0) {}

    void update(f32 delta) {
        if (enemies->size() < 2) {
            f32 x = random_real() < 0.5 ? WORLD_LEFT_EDGE : WORLD_RIGHT_EDGE;
            f32 y = random_real(-20, 20);
            enemies->push_back(new Banana(V2(x, y)));
        }
	if (enemies->size() < 4){
	    f32 x = random_real(WORLD_LEFT_EDGE,WORLD_RIGHT_EDGE);
            f32 y = WORLD_TOP_EDGE;
            enemies->push_back(new TrashBag(V2(x, y)));
	}
    }

    std::vector<Enemy*>* enemies;
    u32 threat;
};

Renderer::ParticleSystem hit_particles;
std::vector<Enemy*> enemies;
Spawner spawner(&enemies);

void initalize_enemies() {
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
    hit_particles.spawn_size = {2.0, 3.0};
    u32 count = random_int() % 10 + 15;
    for (u32 i = 0; i < count; i++) {
        hit_particles.spawn();
    }
    hit_particles.spawn_size = old;
}

void update_enemies(f32 delta) {
    hit_particles.update(delta);
    for (s32 i = enemies.size() - 1; i >= 0; i--) {
        enemies[i]->update(delta);
        if (enemies[i]->is_dead()) {
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
        Physics::debug_draw_body(&body);
    }
}
