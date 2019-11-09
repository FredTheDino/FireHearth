struct HighScore {
    std::string name;
    u32 score;
};

std::vector<HighScore> read_highscores() {
    std::vector<HighScore> scores;

    std::ifstream input("highscores.txt", std::ifstream::in);

    if (input.fail()) {
        return scores;
    }

    std::string name;
    int score;
    while (input >> name) {
        input >> score;
        scores.push_back({ name, score });
    }

    return scores;
}

void write_highscores(std::vector<HighScore>& scores, std::string name, int score) {
    HighScore new_score = { name, score };

    s32 index = scores.size();
    for (s32 i = 0; i < scores.size(); i++) {
        if (scores[i].score <= new_score.score) {
            index = i;
            break;
        }
    }
    scores.insert(scores.begin() + index, new_score);

    std::ofstream output("highscores.txt", std::ofstream::out);
    for (HighScore& score : scores) {
        output << score.name << " " << score.score << "\n";
    }
}
