#include "../../include/logic/entities/Collectable.h"

namespace logic {

Collectable::Collectable(float x, float y, float width, float height) : EntityModel(x, y, width, height) {}

/**
 * @brief Template method for collision handling
 *
 * Sequence:
 * 1. onCollected() - derived class notifies specific event
 * 2. markDead() - common cleanup (flags for removal)
 *
 * Called by World::checkCollisions() when PacMan overlaps collectable.
 */
void Collectable::onCollisionWithPacMan() {
    onCollected();
    markDead();
}

} // namespace logic