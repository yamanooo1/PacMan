#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H
#include "../core/EntityModel.h"

namespace logic {

/**
 * @brief Player-controlled entity in the PacMan game
 *
 * PacMan is the main player character that navigates the maze, collects coins,
 * eats fruits to activate fear mode, and avoids or eats ghosts.
 *
 * Movement System:
 * - Desired direction: Player's input intention (can be blocked by walls)
 * - Current direction: Actual movement direction being executed
 * - Turn execution: Happens when aligned with grid and desired path is clear
 *
 * Lifecycle:
 * - Spawn → Active → Dead → Respawn at spawn point
 * - Lives managed by Lives system (observes PACMAN_DIED events)
 * - Death animation triggered before respawn
 *
 * Collision Behavior:
 * - Coins: Collected (10-30 points with decay)
 * - Fruits: Collected (50 points) + activates fear mode
 * - Ghosts (normal): Dies, loses a life
 * - Ghosts (fear mode): Ghost dies, +200 points
 * - Walls: Movement blocked
 *
 * Observer Pattern:
 * - Notifies observers on DIRECTION_CHANGED (for animation)
 * - Notifies observers on PACMAN_DIED (for death animation + sound)
 * - Notifies observers on WALL_HIT (optional sound effect)
 */
class PacMan : public EntityModel {
private:
    float speed;                    // Movement speed in grid units per second
    Direction desiredDirection;     // Player's input (may be blocked by wall)
    float spawnX, spawnY;          // Respawn position after death

    /**
     * @brief Check if PacMan is aligned with grid center
     *
     * Used to determine if turning is allowed (prevents diagonal movement).
     * PacMan can only change direction when near a grid cell center.
     *
     * @return true if aligned within tolerance, false otherwise
     */
    [[nodiscard]] bool isAlignedWithGrid() const;

public:
    /**
     * @brief Construct PacMan at given position
     *
     * @param x Initial X position in grid coordinates
     * @param y Initial Y position in grid coordinates
     *
     * Initial state:
     * - Size: 0.8x0.8 grid units (smaller than walls for smooth movement)
     * - Speed: 4.0 units/second
     * - Direction: LEFT (classic PacMan starting direction)
     * - Desired direction: LEFT
     */
    PacMan(float x, float y);
    ~PacMan() override = default;

    /**
     * @brief Set the direction player wants to move
     *
     * This stores the player's input intention. Actual direction change
     * happens in World::updatePacMan() when movement is valid.
     *
     * @param dir Desired movement direction from player input
     */
    void setDesiredDirection(Direction dir);
    [[nodiscard]] Direction getDesiredDirection() const { return desiredDirection; }

    /**
     * @brief Execute the turn to desired direction (if valid)
     *
     * Changes current direction to desired direction.
     * Should only be called after validating the turn is possible.
     *
     * @note Typically called by World after collision checks
     */
    void executeTurn();

    /**
     * @brief Update PacMan position based on current direction
     *
     * @param deltaTime Time elapsed since last update (seconds)
     * @param canMove Whether movement is allowed (false during death/ready state)
     *
     * Movement calculation:
     * - new_position = current_position + (direction * speed * deltaTime)
     * - Grid-based: position 10.0 = center of grid cell 10
     */
    void update(float deltaTime, bool canMove);

    // Speed management
    [[nodiscard]] float getSpeed() const { return speed; }
    void setSpeed(float s) { speed = s; }

    /**
     * @brief Trigger death sequence
     *
     * Notifies PACMAN_DIED event which triggers:
     * - Death animation in PacManView
     * - Life reduction in Lives system
     * - Death sound effect
     * - Movement sound stops
     */
    void die();

    /**
     * @brief Respawn PacMan at spawn position
     *
     * Resets state after death animation completes:
     * - Position reset to spawn point
     * - Direction reset to LEFT
     * - Desired direction cleared
     * - Notifies DIRECTION_CHANGED to reset animation
     */
    void respawn();

    /**
     * @brief Set the spawn position for respawning after death
     *
     * @param x Spawn X coordinate
     * @param y Spawn Y coordinate
     */
    void setSpawnPosition(float x, float y);

    /**
     * @brief Notify observers that direction changed
     *
     * Used to update view animation when direction changes.
     * Triggers sprite rotation and mouth animation reset.
     */
    void notifyDirectionChange();

    /**
     * @brief Notify observers that PacMan hit a wall
     *
     * Optional sound effect trigger when movement is blocked.
     */
    void hitWall();
};

} // namespace logic

#endif