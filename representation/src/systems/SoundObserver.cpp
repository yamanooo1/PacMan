#include "../../include/representation/systems/SoundObserver.h"

namespace representation {

using logic::GameEvent;

SoundObserver::SoundObserver() {}

void SoundObserver::update(logic::GameEvent event) {
    SoundManager& soundManager = SoundManager::getInstance();

    switch (event) {
    case logic::GameEvent::COIN_COLLECTED:
        break;

    case logic::GameEvent::FRUIT_COLLECTED:
        soundManager.playSound(SoundEffect::FRUIT_COLLECTED);
        soundManager.startFearModeSound();
        break;

    case logic::GameEvent::GHOST_EATEN:
        soundManager.playSound(SoundEffect::GHOST_EATEN);
        break;

    case logic::GameEvent::PACMAN_DIED:
        soundManager.playSound(SoundEffect::PACMAN_DIED);
        soundManager.stopFearModeSound();
        break;

    case logic::GameEvent::LEVEL_CLEARED:
        break;

    case logic::GameEvent::DIRECTION_CHANGED:
        break;

    default:
        break;
    }
}

} // namespace representation