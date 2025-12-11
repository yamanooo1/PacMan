#include "../../include/logic/systems/Lives.h"

Lives::Lives(int startingLives)
    : remainingLives(startingLives)
    , maxLives(startingLives) {
}

void Lives::update(GameEvent event) {
  if (event == GameEvent::PACMAN_DIED) {
    remainingLives--;
  }
}

void Lives::reset(int lives) {
  remainingLives = lives;
  maxLives = lives;
}