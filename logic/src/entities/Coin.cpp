#include "../../include/logic/entities/Coin.h"

Coin::Coin(float x, float y) : Collectable(x, y, 0.02f, 0.02f) {}

void Coin::onCollected() { notify(GameEvent::COIN_COLLECTED); }