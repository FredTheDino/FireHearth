Vec4 TEXT_COORDINATES[] = {
    V4( 1, 16, 13, 14), // G
    V4(15, 16, 11, 14), // A
    V4(27, 16, 13, 14), // M
    V4(40, 16,  9, 14), // E
    V4( 0,  0, 10,  0), // 
    V4(50, 16, 12, 14), // O
    V4(63, 16, 13, 14), // V
    V4(40, 16,  9, 14), // E
    V4(77, 16, 10, 14), // R
};

f32 get_game_over_width() {
    f32 width = 0;
    for (u32 i = 0; i < LEN(TEXT_COORDINATES); i++) {
        width += TEXT_COORDINATES[i].z * PIXEL_TO_WORLD + 1.0;
    }
    return width;
}

void draw_game_over() {
    f32 time = Logic::now();
    Vec2 camera = Renderer::global_camera.position;
    Vec2 position = V2(get_game_over_width() / 2.0, 0);
    for (u32 i = 0; i < LEN(TEXT_COORDINATES); i++) {
        Vec4 coords = TEXT_COORDINATES[i];
        Vec2 dim = V2(coords.z, -coords.w) * PIXEL_TO_WORLD;
        if (dim.y) {
            Renderer::push_sprite(-camera - position + V2(0, sin(time + i) * 2),
                                  dim, 0,
                                  ASSET_PARTICLE_SPRITESHEEP,
                                  V2(coords.x, coords.y),
                                  V2(coords.z, coords.w));
        }
        position -= V2(dim.x + 1.0, 0.0);
    }
}


