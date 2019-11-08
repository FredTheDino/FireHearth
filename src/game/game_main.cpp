// Tell the engine that this is loaded
#define FOG_GAME
#define NO_ASSET 1024

#include <vector>
#include "entity.h"

namespace Game {

using namespace Input;

std::vector<Entity> entities;

void setup() {
    entities.push_back({V2(0, 0), 0.3, 0.3, 0.7, ASSET_TEST});
    entities.push_back({V2(0.5, 0.5), 0.3, 0.3, 0, ASSET_TEST});
}

// Main logic
void update(f32 delta) {

}

// Main draw
void draw() {
    for (Entity entity : entities) {
        if (entity.image != NO_ASSET) {
            Image* img = Asset::fetch_image(entity.image);
            Renderer::push_sprite(entity.pos,
                    V2(entity.width, entity.height),
                    entity.rotation,
                    entity.image,
                    V2(0,0),
                    V2(img->width, img->height));
        } else {
            Renderer::push_rectangle(entity.pos, V2(entity.width, entity.height));
        }
    }
}

}  // namespace Game
