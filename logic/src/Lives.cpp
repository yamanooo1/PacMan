//
// Created by yamanooo on 11/27/25.
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