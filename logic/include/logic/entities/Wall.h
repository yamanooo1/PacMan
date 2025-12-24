#ifndef PACMAN_WALL_H
#define PACMAN_WALL_H
#include "../core/EntityModel.h"

namespace logic {

/**
 * @brief Static obstacle entity that blocks movement
 *
 * Walls define the maze structure and constrain movement.
 * They are purely structural - no collision events or special behavior.
 *
 * Movement Blocking:
 * - PacMan: Stops at wall, changes direction blocked
 * - Ghosts: Avoid walls in pathfinding decisions
 * - Collision detection: World::hasWallInGridCell() checks walls
 *
 * Visual Representation:
 * - Size: 1.0x1.0 grid units (full cell coverage)
 * - Color: Blue (WallView renders as rectangles)
 * - No sprites used (solid color fill)
 *
 * Map Notation:
 * - 'x' in map file creates a wall at that position
 *
 * Size Detection:
 * - World uses size to identify walls: width >= 0.9 && height >= 0.9
 * - Entities (0.8x0.8) and collectables (0.02-0.03) have smaller sizes
 *
 * Design Pattern:
 * - Inherits from EntityModel but doesn't use Subject features
 * - No collision events (onCollisionWithPacMan() not overridden)
 * - Static presence in entity list
 */
class Wall : public EntityModel {
public:
    /**
     * @brief Construct a wall at grid position with specified dimensions
     *
     * @param x Grid X position
     * @param y Grid Y position
     * @param width Wall width (typically 1.0)
     * @param height Wall height (typically 1.0)
     *
     * @note Walls are typically 1.0x1.0 for full grid cell coverage
     */
    Wall(float x, float y, float width, float height);
    ~Wall() override = default;
};

} // namespace logic

#endif