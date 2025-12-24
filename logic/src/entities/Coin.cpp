#include "../../include/logic/entities/Coin.h"

namespace logic {

/**
 * @brief Standard coin size: 0.02x0.02 grid units
 *
 * Small enough to be scattered densely in corridors,
 * large enough for visible collision detection.
 */
Coin::Coin(float x, float y) : Collectable(x, y, 0.02f, 0.02f) {}

/**
 * @brief Notify observers of coin collection
 *
 * Score observer calculates points based on collection speed.
 * SoundObserver plays "wakka wakka" eating sound.
 */
void Coin::onCollected() { notify(GameEvent::COIN_COLLECTED); }

} // namespace logic