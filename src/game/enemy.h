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
	Enemy(pos, V2(10,10), 0, 1),
	velocity(V2(0,0)),
	orig_pos(pos) {
	    animation_delay = 0.5;
	    images.push_back(ASSET_TRASH_SLEEP);
	}

    const f32 SPEED = 3;
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
		
	    //animate this.
	    //LOG("%f", time);
	    //LOG("%f", buryTime);
	    LOG("%f", (buryTime));
	    if (buryTime >= 3){
		currentTrashLevel++;
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
	    f32 x = random_real() < 0.5 ? WORLD_LEFT_EDGE : WORLD_RIGHT_EDGE;
            f32 y = WORLD_TOP_EDGE;
            enemies->push_back(new TrashBag(V2(x, y)));
	}
    }

    std::vector<Enemy*>* enemies;
    u32 threat;
};

std::vector<Enemy*> enemies;
Spawner spawner(&enemies);

void update_enemies(f32 delta) {
    for (s32 i = enemies.size() - 1; i >= 0; i--) {
        enemies[i]->update(delta);
        if (enemies[i]->is_dead()) {
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        }
    }
}
