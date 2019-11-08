// Tell the engine that this is loaded
#define FOG_GAME
#define NO_ASSET 1024

namespace Game {

using namespace Input;
Physics::ShapeID square;

#include <vector>
#include "entity.h"
#include "enemy.h"
#include "truck.h"
Truck truck;

std::vector<Enemy*> enemies;

void draw_entity(Entity* entity) {
    if (entity->image != NO_ASSET) {
        Image* img = Asset::fetch_image(entity->image);
        Renderer::push_sprite(entity->pos,
                entity->dim,
                entity->rotation,
                entity->image,
                V2(0,0),
                V2(img->width, img->height));
    } else {
        Renderer::push_rectangle(entity->pos, entity->dim);
    }
}

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

    Enemy* e = new Banana(V2(0, 0));
    enemies.push_back(e);
    e = new Banana(V2(5, 5));
    enemies.push_back(e);
}

// Main logic
void update(f32 delta) {
    truck.update(delta);
    for (Enemy* enemy : enemies) {
        enemy->update(delta);
    }
}

// Main draw
void draw() {
    truck.draw();
    for (Enemy* enemy : enemies) {
        draw_entity(enemy);
    }
}

}  // namespace Game
