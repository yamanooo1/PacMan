#include "../../include/representation/systems/SoundObserver.h"

namespace representation {

using logic::GameEvent;

SoundObserver::SoundObserver() {}

/**
 * @brief Route game events to appropriate sounds
 *
 * Sound decisions:
 *
 * COIN_COLLECTED:
 *   - No direct sound (movement sound handles coin eating)
 *   - LevelState manages movement sound based on collection timing
 *
 * FRUIT_COLLECTED:
 *   - Play fruit collection sound (power-up)
 *   - Start fear mode music loop (ghosts vulnerable)
 *
 * GHOST_EATEN:
 *   - Play ghost eaten sound (satisfying crunch)
 *
 * PACMAN_DIED:
 *   - Play death sound (fail.wav)
 *   - Stop fear mode music (reset audio state)
 *
 * LEVEL_CLEARED:
 *   - No sound here (handled by LevelState transition)
 *
 * DIRECTION_CHANGED:
 *   - No sound (purely visual event)
 */
void SoundObserver::update(logic::GameEvent event) {
    SoundManager& soundManager = SoundManager::getInstance();

    switch (event) {
    case logic::GameEvent::COIN_COLLECTED:
        // Handled by movement sound system
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
        // State transition handles this
        break;

    case logic::GameEvent::DIRECTION_CHANGED:
        // Visual only
        break;

    default:
        break;
    }
}

} // namespace representation