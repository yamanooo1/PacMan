#include "logic/Score.h"
#include "logic/Stopwatch.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
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

    loadHighScores();
}

void Score::update(GameEvent event) {
    Stopwatch& stopwatch = Stopwatch::getInstance();
    // ✅ NO SoundManager here! Sound is handled by SoundObserver in representation layer

    switch (event) {
        case GameEvent::COIN_COLLECTED: {
            float currentTime = stopwatch.getElapsedTime();
            float timeSinceLastCoin = currentTime - lastCoinTime;

            float decayFactor = std::pow(0.9f, timeSinceLastCoin / 0.5f);
            int points = static_cast<int>(30 * decayFactor);
            currentScore += points;

            std::cout << "[SCORE] Coin! +[" << points << " points. Total: " << currentScore << std::endl;
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

// ✅ UPDATED: Load scores with player names
void Score::loadHighScores(const std::string& filepath) {
    scoresFilePath = filepath;
    highScores.clear();

    std::ifstream file(scoresFilePath);
    if (!file.is_open()) {
        std::cout << "[SCORE] No existing high scores file. Starting fresh." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line) && highScores.size() < MAX_HIGH_SCORES) {
        std::istringstream iss(line);
        std::string name;
        int score;

        // Expected format: "NAME SCORE"
        if (iss >> name >> score) {
            highScores.push_back(ScoreEntry(name, score));
        }
    }

    file.close();
    std::cout << "[SCORE] Loaded " << highScores.size() << " high scores." << std::endl;
}

// ✅ UPDATED: Save scores with player names
void Score::saveHighScores() {
    std::ofstream file(scoresFilePath);
    if (!file.is_open()) {
        std::cerr << "[SCORE] Failed to save high scores to: " << scoresFilePath << std::endl;
        return;
    }

    for (const auto& entry : highScores) {
        file << entry.playerName << " " << entry.score << "\n";
    }

    file.close();
    std::cout << "[SCORE] Saved " << highScores.size() << " high scores." << std::endl;
}

bool Score::isHighScore(int score) const {
    if (highScores.size() < MAX_HIGH_SCORES) {
        return true;
    }
    return score > highScores.back().score;  // ✅ Compare with .score
}

// ✅ UPDATED: Add high score with player name
void Score::addHighScore(const std::string& playerName, int score) {
    highScores.push_back(ScoreEntry(playerName, score));

    // Sort in descending order by score
    std::sort(highScores.begin(), highScores.end(),
        [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score;
        });

    // Keep only top 5
    if (highScores.size() > MAX_HIGH_SCORES) {
        highScores.resize(MAX_HIGH_SCORES);
    }

    saveHighScores();
    std::cout << "[SCORE] Added new high score: " << playerName << " - " << score << std::endl;
}