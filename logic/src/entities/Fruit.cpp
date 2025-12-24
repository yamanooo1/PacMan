#include "../../include/logic/entities/Fruit.h"

namespace logic {

Fruit::Fruit(float x, float y) : Collectable(x, y, 0.03f, 0.03f) {}

void Fruit::onCollected() { notify(GameEvent::FRUIT_COLLECTED); }

} // namespace logic