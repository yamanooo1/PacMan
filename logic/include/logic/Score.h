//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_SCORE_H
#define PACMAN_SCORE_H
#include "Observer.h"

class Score: public Observer{

private:
  int currentScore = 0;

  float timeSinceLastCoin = 0.0f;  // Track time since last coin collected

  const int BASE_COIN_VALUE = 20;
  const float DECAY_INTERVAL = 0.5f;  // Decay every 0.5 seconds
  const float DECAY_RATE = 0.9f;      // Lose 10% per interval

public:
  Score() = default;

  // todo
  void update(GameEvent event) override;

  void tick(float deltaTime);

  int getScore() const { return currentScore; }
};


#endif //PACMAN_SCORE_H