//
// SoundObserver.cpp - Plays sounds in response to game events
// FIXED: Removed coin collection sound (file deleted)
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
    // ✅ REMOVED: No sound for coin collection
    // (You deleted the coin eating sound file)
    break;

  case GameEvent::FRUIT_COLLECTED:
    soundManager.playSound(SoundEffect::FRUIT_COLLECTED);
    soundManager.playSound(SoundEffect::FEAR_MODE_START);
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