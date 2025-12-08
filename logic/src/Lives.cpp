//
// Lives.cpp - WITHOUT sound dependencies (sounds handled by SoundObserver)
//

#include "logic/Lives.h"
#include <iostream>

Lives::Lives(int startingLives)
    : remainingLives(startingLives)
    , maxLives(startingLives)
{
}

void Lives::update(GameEvent event) {
  if (event == GameEvent::PACMAN_DIED) {
    remainingLives--;

    std::cout << "[LIVES] PacMan died! Remaining lives: " << remainingLives << std::endl;

    // ✅ NO SoundManager here! Sound is handled by SoundObserver in representation layer

    if (remainingLives <= 0) {
      std::cout << "[LIVES] GAME OVER!" << std::endl;
    }
  }
}

void Lives::reset(int lives) {
  remainingLives = lives;
  maxLives = lives;
  std::cout << "[LIVES] Reset to " << remainingLives << " lives" << std::endl;
}