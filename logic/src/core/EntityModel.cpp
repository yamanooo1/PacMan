#include "../../include/logic/core/EntityModel.h"

EntityModel::EntityModel(float posX, float posY, float w, float h)
    : x(posX), y(posY), width(w), height(h), direction(Direction::NONE), dead(false) {}

void EntityModel::onCollisionWithPacMan() {}