//
// SoundObserver.cpp - Plays sounds in response to game events
//

#include "representation/SoundObserver.h"
#include <iostream>

SoundObserver::SoundObserver() {
  std::cout << "[SoundObserver] Created sound observer" << std::endl;
}

void SoundObserver::update(GameEvent event) {
  SoundManager& soundManager = SoundManager::getInstance();

  switch (event) {
  case GameEvent::COIN_COLLECTED:
    soundManager.playSound(SoundEffect::COIN_COLLECTED);
    break;

  case GameEvent::FRUIT_COLLECTED:
    soundManager.playSound(SoundEffect::FRUIT_COLLECTED);
    soundManager.playSound(SoundEffect::FEAR_MODE_START);  // ✅ Fear mode activates!
    break;

  case GameEvent::GHOST_EATEN:
    soundManager.playSound(SoundEffect::GHOST_EATEN);
    break;

  case GameEvent::PACMAN_DIED:
    soundManager.playSound(SoundEffect::PACMAN_DIED);
    break;

  case GameEvent::LEVEL_CLEARED:
    // No specific sound for level cleared currently
    break;

  case GameEvent::DIRECTION_CHANGED:
    // No sound for direction changes
    break;

  default:
    break;
  }
}