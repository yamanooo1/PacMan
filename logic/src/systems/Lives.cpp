#include "../../include/logic/systems/Lives.h"

namespace logic {

Lives::Lives(int startingLives) : remainingLives(startingLives), maxLives(startingLives) {}

/**
 * @brief Process PACMAN_DIED event and decrement lives
 *
 * Simple death handler: one death = one life lost.
 * No special logic or validation needed.
 */
void Lives::update(GameEvent event) {
    if (event == GameEvent::PACMAN_DIED) {
        remainingLives--;
    }
}

/**
 * @brief Reset lives for new game
 *
 * Resets both current and max lives to given value.
 * Typically called when starting fresh game.
 */
void Lives::reset(int lives) {
    remainingLives = lives;
    maxLives = lives;
}

} // namespace logic