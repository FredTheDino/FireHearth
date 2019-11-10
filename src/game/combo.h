const u32 COMBO_HITS_PER_MULTIPLIER = 10;
const f32 COMBO_TIME = 4.0;

u32 hit_counter = 0;
u32 multiplyer;
u32 score;
f32 combo_time;

void reset_score() {
    multiplyer = 1;
    score = 0;
    combo_time = 0.0;
}

void score_hit() {
    hit_counter += 1;
    if (hit_counter == (COMBO_HITS_PER_MULTIPLIER * multiplyer))
        multiplyer++;
    hit_counter %= (COMBO_HITS_PER_MULTIPLIER * multiplyer);
    combo_time = Logic::now() + COMBO_TIME;
}

void score_hit_enemy() {
    Mixer::play_sound(ASSET_HIT, 0.9, 0.8);
    score_hit();
    score += 5 * multiplyer;
}

void score_kill_enemy() {
    score += 10 * multiplyer;
}

void score_boost_kill_enemy() {
    Mixer::play_sound(ASSET_BOOST_KILL, 0.8, 0.7);
    score_hit();
    score_hit();
    score += 20 * multiplyer;
}

void update_score() {
    if (Logic::now() > combo_time) {
        multiplyer = 1;
        hit_counter = 0;
        // Do reset sound.
    }
}

u32 get_score() {
    return score;
}

u32 get_multiplier() {
    return multiplyer;
}
