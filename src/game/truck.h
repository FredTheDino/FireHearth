const Vec4 TRUCK_SPRITE = V4(1, 4, 15, 9);
const Vec4 BULLET_SPRITE = V4(10, 0, 6, 3);
const f32 BULLET_ALIVE_TIME = 5.0;
const f32 BULLET_SPEED = 100.0;

const f32 TRUCK_MAX_SPEED = 225.0;
const f32 TRUCK_BOOST_STRENGTH = 75.0;
const f32 TRUCK_BOOST_INITIAL = 15.0;
const f32 TRUCK_ROTATION_SPEED = 8.0;
const f32 TRUCK_VELOCITY_DAMPING = 0.4;
const f32 TRUCK_VELOCITY_WEIGHT = 0.2;
const f32 TRUCK_SHOOT_DELAY = 0.1;
const f32 TRUCK_BOOST_MAX = 60.0;
const f32 TRUCK_BOOST_TIME_MAX = 0.5;
const f32 TRUCK_BOOST_TIME_MIN = 0.0;

struct Bullet {
    Physics::Body body;
    f32 spawn_time;
    f32 angle;
    f32 speed = BULLET_SPEED;
    Vec2 dimension = V2(BULLET_SPRITE.z, BULLET_SPRITE.w) * PIXEL_TO_WORLD;
    bool hit_enemy = false;

    void update(f32 delta);

    void draw();

    bool is_dead() const;
};

struct Truck {
    Physics::Body body;
    Renderer::ParticleSystem super_particles;
    Renderer::ParticleSystem boost_particles;
    Renderer::ParticleSystem smoke_particles;
    Vec2 dimension = V2(TRUCK_SPRITE.z, TRUCK_SPRITE.w) * PIXEL_TO_WORLD;
    Vec2 forward = V2(1, 0);
    f32 last_shot = 0;
    f32 boost_timer = TRUCK_BOOST_TIME_MAX;
    bool max_out = false;

    void boost(f32 delta);

    void update(f32 delta);

    void draw();
};

std::vector<Bullet> bullets;
void initalize_bullets();

void create_bullet(Vec2 position, Vec2 forward);

void update_bullets(f32 delta);

void draw_bullets();

Truck create_truck();

