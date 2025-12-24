#include "../../include/logic/systems/Lives.h"

namespace logic {

Lives::Lives(int startingLives) : remainingLives(startingLives), maxLives(startingLives) {}

void Lives::update(GameEvent event) {
    if (event == GameEvent::PACMAN_DIED) {
        remainingLives--;
    }
}

void Lives::reset(int lives) {
    remainingLives = lives;
    maxLives = lives;
}

} // namespace logic