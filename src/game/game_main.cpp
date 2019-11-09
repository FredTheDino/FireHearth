// Tell the engine that this is loaded
#define FOG_GAME
#include <vector>

const u32 NO_ASSET = 1024;
const f32 WORLD_LEFT_EDGE  = -80;
const f32 WORLD_RIGHT_EDGE =  80;
const f32 WORLD_TOP_EDGE = 34;
const f32 WORLD_BOTTOM_EDGE = -34;
const f32 PIXEL_TO_WORLD = 1.0 / 3.0;

namespace Game {

using namespace Input;
Physics::ShapeID square;
bool game_over = false;
Vec2 get_truck_pos();

const float MAX_TRASH_LEVEL = -15;
const float MIN_TRASH_LEVEL = -43;
const float TRASH_VELOCITY = 0.01;

f32 currentTrashLevel = -50;
f32 goalTrashLevel = MIN_TRASH_LEVEL;
f32 groundLevel = currentTrashLevel + 20;

#include "text.h"
// #include "highscore.h"
#include "entity.h"
#include "enemy.h"
#include "truck.h"
#include "truck.cpp"
#include "clouds.h"
#include "gameover.cpp"

Truck truck;

float CASTLE_DISTANCE = 5;
float TRASH_MOUNTAIN_DISTANCE = -0.5;

float CAMERA_MAX = 20;
float CAMERA_MIN = -20;

std::string bitmapFontGuide = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int index = 10;
int space = 0;
std::string name = "AAA";
int score = 0;

bool title_screen = true;

Vec2 get_truck_pos() {
    return truck.body.position;
}

void setup() {
    // Bind wasd
    add(K(w), Player::P1, Name::BOOST);
    add(K(d), Player::P1, Name::UP);
    //add(K(s), Player::P1, Name::DOWN); Add brake?
    add(K(a), Player::P1, Name::DOWN);

    // Bind arrow keys
    add(K(UP), Player::P1, Name::BOOST);
    add(K(RIGHT), Player::P1, Name::UP);
    add(K(DOWN), Player::P1, Name::DOWN); //Add brake?
    add(K(LEFT), Player::P1, Name::DOWN);

    // Shoot!
    add(K(SPACE), Player::P1, Name::SHOOT);

    // Restart
    add(K(r), Player::P1, Name::RESTART);

	//Confirm
	add(K(RETURN), Player::P1, Name::CONFIRM); 

    Mixer::play_sound(ASSET_BEEPBOX_SONG, 1.0, 5.0
              ,Mixer::AUDIO_DEFAULT_VARIANCE, Mixer::AUDIO_DEFAULT_VARIANCE, true);

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

    initalize_enemies();
    createCloudSystems();

    Renderer::global_camera.zoom = 3.335 / 200.0;

    Logic::add_callback(Logic::At::PRE_UPDATE, spawnCloud, 0, Logic::FOREVER, 2);
}

void camera_follow(Vec2 target, f32 delta) {
    target.x = CLAMP(CAMERA_MIN, CAMERA_MAX, -target.x);
    Vec2 curr = Renderer::global_camera.position;
    Renderer::global_camera.position.x = LERP(curr.x, 0.05, target.x);
}

Vec2 paralax(Vec2 position, f32 distance) {
    return position - Renderer::global_camera.position / distance;
}

// Main logic
void update(f32 delta) {

	if (title_screen) {
		if (pressed(Player::P1, Name::CONFIRM)) {
			title_screen = false;
		}
	}

    if (game_over) {
		if (pressed(Player::P1, Name::BOOST)) {
			index += 1;
			index = index % 36;
		}
		if (pressed(Player::P1, Name::DOWN)) {
			index -= 1;
			index = index % 36;
		}
		if (pressed(Player::P1, Name::CONFIRM)) {
			index = 10;
			if (space < 2) {
				space += 1;
				name[space] = bitmapFontGuide[index];
			}
			else {
				name[space] = bitmapFontGuide[index];
				// write_highscore(read_highscores(), name, score);
				title_screen = true;
			}
		}
        if (pressed(Player::P1, Name::RESTART)) {
            game_over = false;
            initalize_enemies();
            truck = create_truck();
            currentTrashLevel = -50;
            goalTrashLevel = MIN_TRASH_LEVEL;
            groundLevel = currentTrashLevel + 20;
        }
        return;
    }

    // Update game elements
    truck.update(delta);
    update_bullets(delta);
    spawner.update(delta);
    update_enemies(delta);


    for (Enemy* enemy : enemies) {
        Physics::Body enemy_body = enemy->get_body();
        if (Physics::check_overlap(&enemy_body, &truck.body)){
            game_over = true;
        }
    }

    // Check for bullet collisions
    for (Bullet& bullet : bullets) {
        for (Enemy* enemy : enemies) {
            Physics::Body enemy_body = enemy->get_body();
            if (check_overlap(&bullet.body, &enemy_body)) {
                bullet.hit_enemy = true;
                enemy->hp -= 1;
                emit_hit_particles(bullet.body.position);
            }

            if (Physics::check_overlap(&enemy_body, &truck.body)){
                game_over = true;
            }
        }

    }

    if (down(Player::P1, Name::BOOST)) {
        Renderer::global_camera.shake = random_unit_vec2() * 0.001;
    } else {
        Renderer::global_camera.shake = V2(0, 0);
    }

    camera_follow(truck.body.position, delta);

    updateClouds(delta);

    if (currentTrashLevel >= MAX_TRASH_LEVEL) {
        game_over = true;
    } else if (currentTrashLevel < goalTrashLevel){
        currentTrashLevel += TRASH_VELOCITY;
    }
}

// Main draw
void draw() {
    Renderer::push_sprite(paralax(V2(0, 0), 1.0), V2(120, -67), 0,
                          ASSET_BACKGROUND, V2(0, 0), V2(120, 67));
    drawClouds();

    Renderer::push_sprite(paralax(V2(0, -0.5), CASTLE_DISTANCE), V2(43, -66), 0,
                          ASSET_CASTLE, V2(0, 0), V2(43, 66));

    // Draw trash mountain.
    Renderer::push_sprite(
        V2(60, currentTrashLevel),
        V2(120, -37), 0, ASSET_TRASH_MOUNTAIN, V2(0, 0), V2(120, 37));
    Renderer::push_sprite(
        V2(-60, currentTrashLevel),
        V2(120, -37), 0, ASSET_TRASH_MOUNTAIN, V2(0, 0), V2(120, 37));


    if (game_over) {
        Vec2 dim = messure_text("GAME OVER", 1.0);
        draw_text("GAME OVER", -Renderer::global_camera.position - 
                                V2(dim.x / 2, 0.0), 
                  1.0, sin(Logic::now() / 10) * 0.5);
        // draw_game_over();
    } else {
        truck.draw();
        draw_bullets();
        draw_enemies();
    }
}

}  // namespace Game
