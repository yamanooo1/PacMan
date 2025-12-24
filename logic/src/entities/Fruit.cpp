#include "../../include/logic/entities/Fruit.h"

namespace logic {

/**
 * @brief Standard fruit size: 0.03x0.03 grid units
 *
 * Larger than coins (0.02) for distinction and visibility.
 * Size also helps collision detection distinguish fruit from coins.
 */
Fruit::Fruit(float x, float y) : Collectable(x, y, 0.03f, 0.03f) {}

/**
 * @brief Notify observers and activate fear mode
 *
 * Event cascade:
 * 1. Score adds 50 points
 * 2. World detects fruit collision → activateFearMode()
 * 3. All ghosts enter fear state
 * 4. SoundObserver plays fear mode music
 */
void Fruit::onCollected() { notify(GameEvent::FRUIT_COLLECTED); }

} // namespace logic