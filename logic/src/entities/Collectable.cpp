#include "../../include/logic/entities/Collectable.h"

namespace logic {

Collectable::Collectable(float x, float y, float width, float height) : EntityModel(x, y, width, height) {}

void Collectable::onCollisionWithPacMan() {
    onCollected();
    markDead();
}

} // namespace logic