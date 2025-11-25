//
// Created by yamanooo on 11/22/25.
//

#include "logic/Score.h"
#include <cmath>

void Score::update(GameEvent event) {
  switch (event) {

    case GameEvent::COIN_COLLECTED: {
      float decayFactor = std::pow(0.9f, timeSinceLastCoin / 0.5f);
      currentScore += static_cast<int>(20 * decayFactor);
      timeSinceLastCoin = 0.0f;
      break;
    }

    case GameEvent::GHOST_EATEN: {
      currentScore += 200;
      break;
    }

    case GameEvent::FRUIT_COLLECTED: {
      currentScore += 50;
      break;
    }

    case GameEvent::LEVEL_CLEARED: {
      currentScore += 1000;
      break;
    }
  }
}

void Score::tick(float deltaTime) {}