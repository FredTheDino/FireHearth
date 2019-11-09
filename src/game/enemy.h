struct Enemy : public Entity {
    Enemy(Vec2 pos, Vec2 dim, f32 rotation, u32 hp) :
        Entity(pos, dim, ASSET_TEST, rotation),
        hp(hp),
        time(0),
        animation_delay(1) {}

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

    const f32 SPEED = 1.5;
    bool onGround = false;

    void update(f32 delta) {
    
	time += delta;
	f32 buryTime; 
	animate(time);

	LOG("%f",pos.y);
        LOG("%f",groundLevel);
	
	if(pos.y <= groundLevel){
	    buryTime = (!onGround) ? time : buryTime;
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
	    rotation = sin(time*2) /5;
		
	    //animate this.
	    LOG("%f", time);
	    LOG("%f", buryTime);
	    if (time - buryTime >= 5){
		currentTrashLevel++;
		groundLevel++;
		//kill enemies
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
            f32 y = random_real(-5, 5);
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
