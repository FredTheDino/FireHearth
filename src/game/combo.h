const u32 COMBO_HITS_PER_MULTIPLIER = 10;
const f32 COMBO_TIME = 5.0;

u32 hit_counter = 0;
u32 multiplier;
u32 score;
f32 combo_time;

void reset_score() {
    multiplier = 1;
    score = 0;
    combo_time = 0.0;
}

void score_hit(u32 hits = 1) {
    hit_counter += 1;
    if (hit_counter == (COMBO_HITS_PER_MULTIPLIER * multiplier))
        multiplier++;
    hit_counter %= (COMBO_HITS_PER_MULTIPLIER * multiplier);
    combo_time = Logic::now() + COMBO_TIME;
}

void score_hit_enemy() {
    Mixer::play_sound(ASSET_HIT, 0.9, 0.8);
    score_hit();
    score += 5 * multiplier;
}

void score_kill_enemy() {
    score += 10 * multiplier;
}

void score_boost_kill_enemy() {
    Mixer::play_sound(ASSET_BOOST_KILL, 0.8, 0.7);
    score_hit(10);
    score += 20 * multiplier;
    multiplier++;
}

void update_score() {
    if (Logic::now() > combo_time) {
        multiplier = 1;
        hit_counter = 0;
        // Do reset sound.
    }
}

u32 get_score() {
    return score;
}

u32 get_multiplier() {
    return multiplier;
}
