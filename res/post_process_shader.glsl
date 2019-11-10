uniform sampler2D screen_sampler;

#ifdef VERT

layout (location=0) in vec2 pos;
layout (location=1) in vec2 uv;

out vec2 pass_uv;
out vec4 pass_color;

void main() {
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
    pass_uv = vec2(uv);
}

#else

in vec2 pass_uv;
in vec4 pass_color;

out vec4 color;

void main() {
    vec2 pixel_offset = vec2(0, 0); // vec2(shake.x, shake.y);
    color = texture(screen_sampler, pass_uv + pixel_offset);
    color = color;
}

#endif
