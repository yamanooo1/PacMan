#include "../../include/representation/systems/SoundObserver.h"

SoundObserver::SoundObserver() {}

void SoundObserver::update(GameEvent event) {
    SoundManager& soundManager = SoundManager::getInstance();

    switch (event) {
    case GameEvent::COIN_COLLECTED:
        break;

    case GameEvent::FRUIT_COLLECTED:
        soundManager.playSound(SoundEffect::FRUIT_COLLECTED);
        soundManager.startFearModeSound();
        break;

    case GameEvent::GHOST_EATEN:
        soundManager.playSound(SoundEffect::GHOST_EATEN);
        break;

    case GameEvent::PACMAN_DIED:
        soundManager.playSound(SoundEffect::PACMAN_DIED);
        soundManager.stopFearModeSound();
        break;

    case GameEvent::LEVEL_CLEARED:
        break;

    case GameEvent::DIRECTION_CHANGED:
        break;

    default:
        break;
    }
}