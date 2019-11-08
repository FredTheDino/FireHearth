// Tell the engine that this is loaded
#define FOG_GAME

#if 0

namespace Game {
using namespace Input;

Physics::ShapeID square;

#include "truck.h"
Truck truck;

void setup() {
    add(K(w), Player::P1, Name::UP);
    add(K(SPACE), Player::P1, Name::BOOST);
    add(K(m), Player::P1, Name::SHOOT);

    {
        Vec2 points[] = {
            V2(-0.5, -0.5),
            V2( 0.5, -0.5),
            V2( 0.5,  0.5),
            V2(-0.5,  0.5),
        };
        square = Physics::add_shape(LEN(points), points)
    }

    truck = create_truck();
}

// Main logic
void update(f32 delta) {
    truck.update();
}

// Main draw
void draw() {
    truck.draw();
}
#endif

}  // namespace Game
