#ifndef PACMAN_SCORE_H
#define PACMAN_SCORE_H
#include "../patterns/Observer.h"
#include <memory>
#include <string>
#include <vector>

namespace logic {

/**
 * @brief Score entry for high score leaderboard
 */
struct ScoreEntry {
    std::string playerName;
    int score;
    ScoreEntry(std::string name = "PLAYER", int s = 0);
};

/**
 * @brief Score tracking system with decay mechanics and high score persistence
 *
 * Scoring Rules:
 * - Coin: 10-30 points (exponential decay based on collection speed)
 * - Fruit: 50 points (fixed)
 * - Ghost: 200 points (fixed)
 * - Level: 1000 points (fixed)
 * - Time penalty: -10 points/second
 *
 * Coin Decay Formula: points = 30 * (0.9 ^ (timeSince / 0.5))
 * Fast collection (0.3s): 30, 27, 27... = momentum bonus
 * Slow collection (2.0s): 30, 20, 20... = penalty
 *
 * Observer Pattern:
 * - Observes: Coins, Fruits, Ghosts
 * - Events: COIN_COLLECTED, FRUIT_COLLECTED, GHOST_EATEN, LEVEL_CLEARED
 */
class Score : public Observer, public std::enable_shared_from_this<Score> {
private:
    int currentScore;
    float lastCoinTime;
    float lastUpdateTime;
    float pointsLostAccumulator;

    static constexpr int MAX_HIGH_SCORES = 5;
    std::vector<ScoreEntry> highScores;
    std::string scoresFilePath;

public:
    Score();
    ~Score() override = default;

    void update(GameEvent event) override;
    void updateScoreDecay();

    int getScore() const { return currentScore; }
    void reset();

    void loadHighScores(const std::string& filepath = "../../resources/data/highscores.txt");
    void saveHighScores() const;
    bool isHighScore(int score) const;
    void addHighScore(const std::string& playerName, int score);
    std::vector<ScoreEntry> getHighScores() const { return highScores; }
    void setScore(int newScore) { currentScore = newScore; }
};

} // namespace logic

#endif