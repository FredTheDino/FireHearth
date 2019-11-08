// Tell the engine that this is loaded
#define FOG_GAME
#define NO_ASSET 1024

#include <vector>
#include "entity.h"
#include "enemy.h"

namespace Game {

using namespace Input;

std::vector<Entity> entities;
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
    Enemy* e = new Banana(V2(0, 0));
    enemies.push_back(e);
    e = new Banana(V2(0.5, 0.5));
    enemies.push_back(e);
}

// Main logic
void update(f32 delta) {
    for (Enemy* enemy : enemies) {
        enemy->update(delta);
    }
}

// Main draw
void draw() {
    for (Enemy* enemy : enemies) {
        draw_entity(enemy);
    }
}

}  // namespace Game
