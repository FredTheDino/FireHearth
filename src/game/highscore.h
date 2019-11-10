struct HighScore {
    std::string name;
    u32 score;

    bool operator< (const HighScore &other) const {
        return score < other.score;
    }
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
        scores.push_back({ name, (u32) score });
    }


    return scores;
}

void write_highscores(std::vector<HighScore>& scores, std::string name, int score) {
    HighScore new_score = { name, (u32) score };
    scores.insert(std::upper_bound(scores.begin(), scores.end(), new_score), new_score);

    std::ofstream output("highscores.txt", std::ofstream::out);
    for (HighScore& score : scores) {
        output << score.name << " " << score.score << "\n";
    }
}
