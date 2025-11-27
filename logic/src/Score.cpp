//
// Created by yamanooo on 11/22/25.
//

#include "logic/Score.h"
#include "logic/Stopwatch.h"
#include <cmath>
#include <iostream>

Score::Score()
    : currentScore(0)
    , lastCoinTime(0.0f)
    , lastUpdateTime(0.0f)
{
    // Initialize timestamps with current elapsed time
    Stopwatch& stopwatch = Stopwatch::getInstance();
    lastCoinTime = stopwatch.getElapsedTime();
    lastUpdateTime = stopwatch.getElapsedTime();
}

void Score::update(GameEvent event) {
    Stopwatch& stopwatch = Stopwatch::getInstance();

    switch (event) {
        case GameEvent::COIN_COLLECTED: {
            // Calculate time since last coin
            float currentTime = stopwatch.getElapsedTime();
            float timeSinceLastCoin = currentTime - lastCoinTime;

            // Apply exponential decay based on time
            float decayFactor = std::pow(0.9f, timeSinceLastCoin / 0.5f);
            int points = static_cast<int>(30 * decayFactor);
            currentScore += points;

            std::cout << "[SCORE] Coin! Time since last: " << timeSinceLastCoin
                      << "s, Decay: " << decayFactor
                      << ", Points: " << points
                      << ", Total: " << currentScore << std::endl;

            // Update last coin time
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
    }
}

void Score::updateScoreDecay() {
    Stopwatch& stopwatch = Stopwatch::getInstance();
    float currentTime = stopwatch.getElapsedTime();
    float deltaTime = currentTime - lastUpdateTime;

    // Accumulate fractional points lost (10 points per second)
    pointsLostAccumulator += 10.0f * deltaTime;

    // Only subtract when we've accumulated at least 1 point
    int pointsToSubtract = static_cast<int>(pointsLostAccumulator);
    if (pointsToSubtract > 0) {
        currentScore -= pointsToSubtract;
        pointsLostAccumulator -= pointsToSubtract;  // Keep the fractional remainder

        // Don't go negative
        if (currentScore < 0) {
            currentScore = 0;
            pointsLostAccumulator = 0.0f;  // Reset accumulator when hitting zero
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