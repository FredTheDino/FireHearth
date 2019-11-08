// Tell the engine that this is loaded
#define FOG_GAME

namespace Game {

using namespace Input;
Physics::ShapeID square;

#include "truck.h"
Truck truck;

void setup() {
    add(K(w), Player::P1, Name::UP);
    add(K(d), Player::P1, Name::UP);
    add(K(s), Player::P1, Name::DOWN);
    add(K(a), Player::P1, Name::DOWN);
    add(K(SPACE), Player::P1, Name::BOOST);
    add(K(m), Player::P1, Name::SHOOT);

    Renderer::set_window_size(800, 600);
    Renderer::set_window_position(200, 100);

    {
        Vec2 points[] = {
            V2(-0.5, -0.5),
            V2( 0.5, -0.5),
            V2( 0.5,  0.5),
            V2(-0.5,  0.5),
        };
        square = Physics::add_shape(LEN(points), points);
    }

    truck = create_truck();

    Renderer::global_camera.zoom = 1.0 / 20.0;
}

// Main logic
void update(f32 delta) {
    truck.update(delta);
}

// Main draw
void draw() {
    truck.draw();
}

}  // namespace Game
