// Tell the engine that this is loaded
#define FOG_GAME
#define NO_ASSET 1024
#define WORLD_LEFT_EDGE -20
#define WORLD_RIGHT_EDGE 20
#define WORLD_TOP_EDGE 10
namespace Game {

using namespace Input;
Physics::ShapeID square;

Vec2 get_truck_pos();


float MAX_TRASH_LEVEL = -15;
float MIN_TRASH_LEVEL = -43;
    
f32 currentTrashLevel = MIN_TRASH_LEVEL;
f32 groundLevel = currentTrashLevel + 11;
    
#include <vector>
#include "entity.h"
#include "enemy.h"
#include "truck.h"
Truck truck;


float CASTLE_DISTANCE = 5;
float TRASH_MOUNTAIN_DISTANCE = 0.5;

float CAMERA_MAX = 20;
float CAMERA_MIN = -20;

std::vector<Enemy*> enemies;
Spawner spawner(&enemies);


Vec2 get_truck_pos() {
    return truck.body.position;
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

void setup() {
    add(K(w), Player::P1, Name::UP);
    add(K(d), Player::P1, Name::UP);
    add(K(s), Player::P1, Name::DOWN);
    add(K(a), Player::P1, Name::DOWN);
    add(K(SPACE), Player::P1, Name::BOOST);
    add(K(m), Player::P1, Name::SHOOT);

    Renderer::set_window_size(1200, 670);
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

    Renderer::global_camera.zoom = 3.335 / 200.0;
}

// Main logic
void update(f32 delta) {

    // Update game elements
    truck.update(delta);
    update_bullets(delta);
    spawner.update(delta);
    for (Enemy* enemy : enemies) {
        enemy->update(delta);
    }

    if (-truck.body.position.x > CAMERA_MAX) {
        Renderer::global_camera.position.x = CAMERA_MAX;
    }
    else if (-truck.body.position.x < CAMERA_MIN) {
        Renderer::global_camera.position.x = CAMERA_MIN;
    } else {
        Renderer::global_camera.position.x = LERP(Renderer::global_camera.position.x, 0.5, -truck.body.position.x);
    }

    if(currentTrashLevel > MAX_TRASH_LEVEL){
	//Game_Over
    }

}

// Main draw
void draw() {
    Renderer::push_sprite(-Renderer::global_camera.position, V2(120, -67), 0,
            ASSET_BACKGROUND, V2(0, 0), V2(120, 67));
    Renderer::push_sprite(V2(-Renderer::global_camera.position.x /
                             CASTLE_DISTANCE, -0.5), V2(43, -66), 0,
            ASSET_CASTLE, V2(0, 0), V2(43, 66));

    truck.draw();
    draw_bullets();
    for (Enemy* enemy : enemies) {
        draw_entity(enemy);
    }

    // Draw trash mountain.
    Renderer::push_sprite(V2((Renderer::global_camera.position.x /
                              TRASH_MOUNTAIN_DISTANCE) + 60, currentTrashLevel),
            V2(120, -37), 0, ASSET_TRASH_MOUNTAIN, V2(0, 0), V2(120, 37));
    Renderer::push_sprite(V2((Renderer::global_camera.position.x /
                              TRASH_MOUNTAIN_DISTANCE) - 60, currentTrashLevel),
            V2(120, -37), 0, ASSET_TRASH_MOUNTAIN, V2(0, 0), V2(120, 37));
}

}  // namespace Game
