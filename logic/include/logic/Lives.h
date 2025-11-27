//
// Created by yamanooo on 11/27/25.
//

#ifndef PACMAN_LIVES_H
#define PACMAN_LIVES_H

#include "Observer.h"

class Lives : public Observer {
private:
  int remainingLives;
  int maxLives;

public:
  explicit Lives(int startingLives = 3);
  ~Lives() override = default;

  void update(GameEvent event) override;

  int getLives() const { return remainingLives; }
  int getMaxLives() const { return maxLives; }
  bool isGameOver() const { return remainingLives <= 0; }

  void reset(int lives = 3);
};

#endif // PACMAN_LIVES_H