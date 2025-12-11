#include "../../include/logic/entities/Collectable.h"

Collectable::Collectable(float x, float y, float width, float height)
    : EntityModel(x, y, width, height) {
}

void Collectable::onCollisionWithPacMan() {
  onCollected();
  markDead();
}