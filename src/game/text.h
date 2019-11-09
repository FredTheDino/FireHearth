Vec4 BITMAP_FONT[] = {
    // 0
    V4(119,  0, 11, 15), // 0
    V4(  2,  0, 14, 15), // 1
    V4( 16,  0, 14, 15), // 2
    V4( 30,  0, 13, 15), // 3
    V4( 44,  0, 11, 15), // 4
    V4( 56,  0, 12, 15), // 5
    V4( 69,  0, 13, 15), // 6
    V4( 81,  0, 13, 15), // 7
    V4( 95,  0, 11, 15), // 8
    V4(107,  0, 11, 15), // 9 

    // 10
    V4(  1,  0, 13, 13), // A
    V4( 16, 17, 14, 13), // B
    V4( 31, 17, 15, 13), // C
    V4( 46, 17, 14, 13), // D
    V4( 60, 17, 11, 13), // E
    V4( 71, 17, 13, 13), // F
    V4( 85, 17, 13, 13), // G
    V4( 99, 17, 13, 13), // H
    V4(104, 17,  6, 13), // I
    V4(121, 17, 13, 13), // J
    V4(135, 17, 12, 13), // K
    V4(148, 17, 13, 13), // L
    V4(163, 17, 13, 13), // M
    V4(176, 17, 13, 13), // N
    V4(190, 17, 13, 13), // O
    V4(204, 17, 12, 13), // P
    V4(218, 17, 12, 13), // Q
    V4(234, 17, 11, 13), // R
    V4(244, 17, 13, 13), // S
    V4(258, 17, 12, 13), // T
    V4(276, 17, 12, 13), // U
    V4(286, 17, 14, 13), // V
    V4(298, 17, 13, 13), // W
    V4(313, 17, 15, 13), // X
    V4(328, 17, 14, 13), // Y
    V4(343, 17, 13, 13), // Z

    // 36
    V4(359, 17,  7, 13), // -
    V4(368, 17,  6, 13), // :
    V4(375, 17, 12, 13), // _

    V4(391, 17, 16, 14), // up
    V4(408, 17, 12, 13), // down

    // 41
    V4(  0,  0, 12,  0), // Space
};

Vec4 COLORS[] = { V4(0.957, 0.886, 0.102, 1.0), V4(0.435, 0.106, 0.580, 1.0), };

const u32 NUM_CHARS = LEN(BITMAP_FONT);

u32 char_to_index(char c) {
    if ('0' <= c && c <= '9')
        return (c - '0');
    if ('a' <= c && c <= 'z')
        return (c - 'a') + 10;

    if ('A' <= c && c <= 'Z')
        return (c - 'A') + 10;

    if (c == '-')
        return 37;
    if (c == ':')
        return 38;
    if (c == '_')
        return 39;
    if (c == ' ')
        return 41;

    LOG("*%d*", c);
    return -1;
}

void draw_text(const char *text, Vec2 position, f32 scale = 1.0, f32 wave = 0.0) {
    char c;
    Vec2 pixel_offset = V2(1, -1) * PIXEL_TO_WORLD;
    int i = 0;
    while ((c = *(text++))) {
        i++;
        Vec4 coords = BITMAP_FONT[char_to_index(c)];
        Vec2 dim = V2(coords.z, -coords.w) * PIXEL_TO_WORLD;
        Vec2 offset = V2(0, sin(i + Logic::now())) * wave;
        Renderer::push_sprite(position + offset + pixel_offset,
                              dim, 0,
                              ASSET_BITMAP_FONT,
                              V2(coords.x, coords.y),
                              V2(coords.z, coords.w),
                              COLORS[1]);
        Renderer::push_sprite(position + offset,
                              dim, 0,
                              ASSET_BITMAP_FONT,
                              V2(coords.x, coords.y),
                              V2(coords.z, coords.w),
                              COLORS[0]);
        position += V2(dim.x + 1.0 * PIXEL_TO_WORLD, 0.0);
    }                         
}

