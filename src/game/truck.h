
struct Truck {
    Physics::Body body;
    Vec2 dimension = V2(1, 1);
    Vec2 forward = V2(1, 0);
    f32 max_speed = 225.0;
    f32 boost_strength = 8.0;
    f32 rotation_speed = 5.0;

    void boost(f32 delta) {
        // TODO(ed): When you press the button, a huge fire burst should
        // happen and you should get slighly more speed.
        body.velocity += forward * boost_strength * delta;
    }

    void update(f32 delta) {
        Physics::integrate(&body, delta);
        body.velocity += V2(0, -delta * 5);
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
    }

    void draw() {
        Renderer::push_rectangle(body.position, dimension);

        Renderer::push_line(body.position, body.position + forward, V4(1, 0, 1, 1));
    }
};

Truck create_truck() {
    Truck truck = { Physics::create_body(square, 1.0, 0xFF, 0.0, 0.2) };
    return truck;
}

