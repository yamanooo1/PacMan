//
// SoundObserver.cpp - Plays sounds in response to game events
// FIXED: Fear mode now loops continuously instead of one-shot
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
    // No sound for coin collection (handled by movement sound)
    break;

  case GameEvent::FRUIT_COLLECTED:
    // ✅ Play fruit sound (one-shot)
    soundManager.playSound(SoundEffect::FRUIT_COLLECTED);

    // ✅ Start looping fear mode music (won't be interrupted by fruit sound)
    soundManager.startFearModeSound();
    std::cout << "[SoundObserver] Fruit collected - started looping fear mode music" << std::endl;
    break;

  case GameEvent::GHOST_EATEN:
    soundManager.playSound(SoundEffect::GHOST_EATEN);
    break;

  case GameEvent::PACMAN_DIED:
    soundManager.playSound(SoundEffect::PACMAN_DIED);

    // ✅ Stop fear mode music when PacMan dies
    soundManager.stopFearModeSound();
    std::cout << "[SoundObserver] PacMan died - stopped fear mode music" << std::endl;
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