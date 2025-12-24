#ifndef PACMAN_FRUIT_H
#define PACMAN_FRUIT_H
#include "Collectable.h"

namespace logic {

/**
 * @brief Power pellets that activate fear mode
 *
 * Fruits (power pellets) are strategic collectibles that:
 * - Grant 50 points (fixed, no decay)
 * - Activate fear mode: ghosts become vulnerable for ~7 seconds
 * - Turn ghosts blue and make them flee from PacMan
 * - Allow PacMan to eat ghosts for 200 points each
 *
 * Visual Representation:
 * - Size: 0.03x0.03 grid units (larger than coins)
 * - Color: Green (FruitView renders as circles)
 * - Sprite: Larger fruit sprite from atlas
 *
 * Strategic Value:
 * - Placed in corners for tactical decisions
 * - Creates risk/reward gameplay (chase ghosts vs. safety)
 * - Fear duration scales with level (harder levels = shorter fear)
 *
 * Map Notation:
 * - 'f' in map file creates a fruit at that position
 *
 * @see World::activateFearMode() for fear mode implementation
 */
class Fruit : public Collectable {
public:
    /**
     * @brief Construct a fruit (power pellet) at grid position
     *
     * Standard size: 0.03x0.03 (larger than coins for visibility)
     */
    Fruit(float x, float y);
    ~Fruit() override = default;

    /**
     * @brief Notify fruit collection and trigger fear mode
     *
     * Triggers FRUIT_COLLECTED event which:
     * - Adds 50 points to score
     * - Activates fear mode in World
     * - Starts fear mode sound effect
     * - Turns all ghosts blue and fleeing
     */
    void onCollected() override;
};

} // namespace logic

#endif