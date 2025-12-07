//
// Lives.h - UPDATED with enable_shared_from_this
//

#ifndef PACMAN_LIVES_H
#define PACMAN_LIVES_H

#include "Observer.h"
#include <memory>

// ✅ ADD: Enable shared_from_this for Observer pattern
class Lives : public Observer, public std::enable_shared_from_this<Lives> {
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