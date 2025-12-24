#include "../../include/logic/core/EntityModel.h"

namespace logic {

/**
 * @brief Constructor initializes entity with position, size, and default state
 *
 * All entities start with:
 * - Direction::NONE (stationary until explicitly set)
 * - dead = false (alive and active)
 *
 * @note Size (width/height) is const and cannot be changed after construction
 */
EntityModel::EntityModel(float posX, float posY, float w, float h)
    : x(posX), y(posY), width(w), height(h), direction(Direction::NONE), dead(false) {}

/**
 * @brief Default collision handler (no-op for most entities)
 *
 * Override in derived classes:
 * - Collectable: marks self as dead
 * - Ghost: checks fear mode and triggers game events
 * - Wall: never collides (collision prevented by movement logic)
 */
void EntityModel::onCollisionWithPacMan() {}

} // namespace logic