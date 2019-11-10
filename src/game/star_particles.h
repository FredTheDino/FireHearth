Renderer::ParticleSystem stars;

void createStarSystem() {
    stars = Renderer::create_particle_system(20000, V2(0, 0));
    stars.add_sprite(ASSET_STAR_1, 0, 0 , 12, 12);
    stars.add_sprite(ASSET_STAR_2, 0, 0 , 12, 12);
    stars.add_sprite(ASSET_STAR_3, 0, 0 , 12, 12);
    stars.relative = true;
    stars.one_alpha = true;
    stars.one_size = true;
    stars.alive_time = {10, 10};
    stars.velocity_dir = {PI/2 - 0.15, PI/2 + 0.15};
    stars.angular_velocity = {0.1, PI};
    stars.velocity = {-60, -120};
    stars.acceleration_dir = {3*PI/2, 3*PI/2};
    stars.acceleration = {-30, -30};
    stars.spawn_size = {0.5, 4};
}

void spawnStar() {
    stars.spawn();
}

void updateStars(f32 delta) {
    stars.position = V2(-Renderer::global_camera.position.x, WORLD_BOTTOM_EDGE);
    stars.update(delta);
}

void drawStars() {
    stars.draw();
}
