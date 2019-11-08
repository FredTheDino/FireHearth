struct Enemy : public Entity {
    Enemy(Vec2 pos, Vec2 dim, AssetID image, f32 rotation, u32 hp) :
        Entity(pos, dim, image, rotation),
        hp(hp),
        time(0) {}

    u32 hp;
    f32 time;

    bool is_dead() {
        return hp <= 0;
    }

};

struct Banana : public Enemy {
    Banana(Vec2 pos) : Enemy(pos, V2(0.3, 0.3), ASSET_TEST, 0, 10) {}

    void update(f32 delta) override {
        time += delta;
        pos.y += delta * sin(time) / 20;
    }
};
