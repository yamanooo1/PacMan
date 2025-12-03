#include "logic/Score.h"
#include "logic/Stopwatch.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>

Score::Score()
    : currentScore(0)
    , lastCoinTime(0.0f)
    , lastUpdateTime(0.0f)
    , scoresFilePath("../../resources/data/highscores.txt")
{
    Stopwatch& stopwatch = Stopwatch::getInstance();
    lastCoinTime = stopwatch.getElapsedTime();
    lastUpdateTime = stopwatch.getElapsedTime();

    // Load existing high scores
    loadHighScores();
}

void Score::update(GameEvent event) {
    Stopwatch& stopwatch = Stopwatch::getInstance();

    switch (event) {
        case GameEvent::COIN_COLLECTED: {
            float currentTime = stopwatch.getElapsedTime();
            float timeSinceLastCoin = currentTime - lastCoinTime;

            float decayFactor = std::pow(0.9f, timeSinceLastCoin / 0.5f);
            int points = static_cast<int>(30 * decayFactor);
            currentScore += points;

            std::cout << "[SCORE] Coin! Time since last: " << timeSinceLastCoin
                      << "s, Decay: " << decayFactor
                      << ", Points: " << points
                      << ", Total: " << currentScore << std::endl;

            lastCoinTime = currentTime;
            break;
        }

        case GameEvent::GHOST_EATEN:
            currentScore += 200;
            std::cout << "[SCORE] Ghost eaten! +200 points. Total: " << currentScore << std::endl;
            break;

        case GameEvent::FRUIT_COLLECTED:
            currentScore += 50;
            std::cout << "[SCORE] Fruit collected! +50 points. Total: " << currentScore << std::endl;
            break;

        case GameEvent::LEVEL_CLEARED:
            currentScore += 1000;
            std::cout << "[SCORE] Level cleared! +1000 points. Total: " << currentScore << std::endl;
            break;

        default:
            break;
    }
}

void Score::updateScoreDecay() {
    Stopwatch& stopwatch = Stopwatch::getInstance();
    float currentTime = stopwatch.getElapsedTime();
    float deltaTime = currentTime - lastUpdateTime;

    pointsLostAccumulator += 10.0f * deltaTime;

    int pointsToSubtract = static_cast<int>(pointsLostAccumulator);
    if (pointsToSubtract > 0) {
        currentScore -= pointsToSubtract;
        pointsLostAccumulator -= pointsToSubtract;

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

// ✅ NEW: Load high scores from file
void Score::loadHighScores(const std::string& filepath) {
    scoresFilePath = filepath;
    highScores.clear();

    std::ifstream file(scoresFilePath);
    if (!file.is_open()) {
        std::cout << "[SCORE] No existing high scores file found. Starting fresh." << std::endl;
        return;
    }

    int score;
    while (file >> score && highScores.size() < MAX_HIGH_SCORES) {
        highScores.push_back(score);
    }

    file.close();
    std::cout << "[SCORE] Loaded " << highScores.size() << " high scores." << std::endl;
}

// ✅ NEW: Save high scores to file
void Score::saveHighScores() {
    std::ofstream file(scoresFilePath);
    if (!file.is_open()) {
        std::cerr << "[SCORE] Failed to save high scores to: " << scoresFilePath << std::endl;
        return;
    }

    for (int score : highScores) {
        file << score << "\n";
    }

    file.close();
    std::cout << "[SCORE] Saved " << highScores.size() << " high scores." << std::endl;
}

// ✅ NEW: Check if score qualifies as high score
bool Score::isHighScore(int score) const {
    if (highScores.size() < MAX_HIGH_SCORES) {
        return true;  // Less than 5 scores, always qualifies
    }

    // Check if score is higher than the lowest high score
    return score > highScores.back();
}

// ✅ NEW: Add a high score to the list
void Score::addHighScore(int score) {
    highScores.push_back(score);

    // Sort in descending order
    std::sort(highScores.begin(), highScores.end(), std::greater<int>());

    // Keep only top 5
    if (highScores.size() > MAX_HIGH_SCORES) {
        highScores.resize(MAX_HIGH_SCORES);
    }

    // Save to file
    saveHighScores();

    std::cout << "[SCORE] Added new high score: " << score << std::endl;
}