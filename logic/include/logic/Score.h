//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_SCORE_H
#define PACMAN_SCORE_H

#include "Observer.h"

class Score : public Observer {
private:
  int currentScore = 0;
  float lastCoinTime = 0.0f;      // Timestamp of last coin collected
  float lastUpdateTime = 0.0f;    // For time-based score decay
  float pointsLostAccumulator = 0.0f;  // Accumulate fractional points

public:
  Score();
  ~Score() override = default;

  void update(GameEvent event) override;

  // Call this every frame to handle time-based score decay
  void updateScoreDecay();

  int getScore() const { return currentScore; }
  void reset();
};

#endif // PACMAN_SCORE_H