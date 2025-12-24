#include "../../include/logic/entities/Wall.h"

namespace logic {

/**
 * @brief Simple constructor - walls have no special initialization
 *
 * No direction, no movement, no observers needed.
 * Just position and size for collision detection.
 */
Wall::Wall(float x, float y, float width, float height) : EntityModel(x, y, width, height) {}

} // namespace logic