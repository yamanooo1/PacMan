#include "../../include/logic/systems/Score.h"
#include "../../include/logic/utils/Stopwatch.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace logic {

ScoreEntry::ScoreEntry(std::string name, int s) : playerName(std::move(name)), score(s) {}

/**
 * @brief Initialize score system with Stopwatch timing
 *
 * Uses Stopwatch singleton for consistent time tracking across game.
 * Initializes coin decay timer to prevent immediate decay on first coin.
 */
Score::Score()
    : currentScore(0), lastCoinTime(0.0f), lastUpdateTime(0.0f), pointsLostAccumulator(0.0f),
      scoresFilePath("../../resources/data/highscores.txt") {
    Stopwatch& stopwatch = Stopwatch::getInstance();
    lastCoinTime = stopwatch.getElapsedTime();
    lastUpdateTime = stopwatch.getElapsedTime();

    loadHighScores();
}

/**
 * @brief Process game events and award points
 *
 * COIN_COLLECTED: Exponential decay based on collection speed
 * - Fast collection (< 0.5s): Near 30 points (momentum bonus)
 * - Slow collection (> 2s): ~10 points (penalty)
 * - Formula: 30 * (0.9 ^ (timeSince / 0.5))
 *
 * Other events: Fixed point values
 */
void Score::update(GameEvent event) {
    Stopwatch& stopwatch = Stopwatch::getInstance();

    switch (event) {
    case GameEvent::COIN_COLLECTED: {
        float currentTime = stopwatch.getElapsedTime();
        float timeSinceLastCoin = currentTime - lastCoinTime;

        // Exponential decay: fast collection = higher points
        float decayFactor = std::pow(0.9f, timeSinceLastCoin / 0.5f);
        int points = static_cast<int>(30 * decayFactor);
        currentScore += points;

        lastCoinTime = currentTime;  // Reset decay timer
        break;
    }

    case GameEvent::GHOST_EATEN:
        currentScore += 200;
        break;

    case GameEvent::FRUIT_COLLECTED:
        currentScore += 50;
        break;

    case GameEvent::LEVEL_CLEARED:
        currentScore += 1000;
        break;

    default:
        break;
    }
}

/**
 * @brief Update continuous time penalty (-10 points/second)
 *
 * Uses fractional accumulator for smooth, accurate deduction:
 * - Accumulates fractional points each frame
 * - Deducts integer points when accumulator >= 1.0
 * - Keeps fractional remainder for next frame
 *
 * Example (60 FPS):
 * - 10 points/sec = 0.166... points/frame
 * - Accumulates over 6 frames (≈1.0 point)
 * - Deducts 1 point every 6 frames (accurate 10/sec)
 */
void Score::updateScoreDecay() {
    Stopwatch& stopwatch = Stopwatch::getInstance();
    float currentTime = stopwatch.getElapsedTime();
    float deltaTime = currentTime - lastUpdateTime;

    // Accumulate fractional points lost
    pointsLostAccumulator += 10.0f * deltaTime;

    // Deduct integer points
    int pointsToSubtract = static_cast<int>(pointsLostAccumulator);
    if (pointsToSubtract > 0) {
        currentScore -= pointsToSubtract;
        pointsLostAccumulator -= static_cast<float>(pointsToSubtract);

        // Clamp to zero (no negative scores from time penalty)
        if (currentScore < 0) {
            currentScore = 0;
            pointsLostAccumulator = 0.0f;
        }
    }

    lastUpdateTime = currentTime;
}

void Score::reset() {
    currentScore = 0;
    pointsLostAccumulator = 0.0f;
    Stopwatch& stopwatch = Stopwatch::getInstance();
    lastCoinTime = stopwatch.getElapsedTime();
    lastUpdateTime = stopwatch.getElapsedTime();
}

/**
 * @brief Load high scores from text file
 *
 * File format: "NAME SCORE" per line
 * Example:
 * ```
 * ALICE 5000
 * BOB 4500
 * ```
 *
 * Behavior:
 * - Missing file: Not an error, empty list
 * - Malformed lines: Skipped silently
 * - Max 5 entries loaded
 */
void Score::loadHighScores(const std::string& filepath) {
    scoresFilePath = filepath;
    highScores.clear();

    std::ifstream file(scoresFilePath);
    if (!file.is_open()) {
        return;  // File doesn't exist yet (first run)
    }

    std::string line;
    while (std::getline(file, line) && highScores.size() < MAX_HIGH_SCORES) {
        std::istringstream iss(line);
        std::string name;
        int score;

        if (iss >> name >> score) {
            highScores.emplace_back(name, score);
        }
    }

    file.close();
}

/**
 * @brief Save high scores to file
 *
 * Overwrites file with current high score list.
 * Called automatically after addHighScore().
 */
void Score::saveHighScores() const {
    std::ofstream file(scoresFilePath);
    if (!file.is_open()) {
        return;  // Can't write file (permissions issue)
    }

    for (const auto& entry : highScores) {
        file << entry.playerName << " " << entry.score << "\n";
    }

    file.close();
}

/**
 * @brief Check if score qualifies for top 5
 *
 * Qualification:
 * - List has < 5 entries: Always qualifies
 * - List has 5 entries: Must beat lowest score
 */
bool Score::isHighScore(int score) const {
    if (highScores.size() < MAX_HIGH_SCORES) {
        return true;
    }
    return score > highScores.back().score;
}

/**
 * @brief Add new high score and save to file
 *
 * Process:
 * 1. Add entry to list
 * 2. Sort descending by score
 * 3. Trim to 5 entries
 * 4. Save to file
 */
void Score::addHighScore(const std::string& playerName, int score) {
    highScores.emplace_back(playerName, score);

    // Sort descending (highest first)
    std::sort(highScores.begin(), highScores.end(),
              [](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; });

    // Keep only top 5
    if (highScores.size() > MAX_HIGH_SCORES) {
        highScores.resize(MAX_HIGH_SCORES);
    }

    saveHighScores();
}

} // namespace logic