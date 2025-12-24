#include "../../include/logic/entities/PacMan.h"
#include <cmath>

namespace logic {

/**
 * @brief Initialize PacMan with standard game values
 *
 * Standard dimensions: 0.8x0.8 allows smooth navigation through 1.0 wide corridors
 * Standard speed: 4.0 units/sec provides good gameplay feel
 * Starting direction: LEFT matches classic PacMan arcade behavior
 */
PacMan::PacMan(float x, float y)
    : EntityModel(x, y, 0.8f, 0.8f), speed(4.0f), desiredDirection(Direction::LEFT), spawnX(x), spawnY(y) {
    setDirection(Direction::LEFT);
}

void PacMan::setDesiredDirection(Direction dir) { desiredDirection = dir; }

/**
 * @brief Execute turn without validation (assumes World checked validity)
 *
 * This method should only be called by World::updatePacMan() after
 * verifying the turn is valid (no wall in desired direction).
 */
void PacMan::executeTurn() {
    if (desiredDirection != Direction::NONE) {
        setDirection(desiredDirection);
    }
}

/**
 * @brief Trigger death event which cascades to multiple observers
 *
 * Event flow:
 * 1. notify(PACMAN_DIED) called
 * 2. PacManView receives event → starts death animation
 * 3. Lives receives event → decrements life counter
 * 4. SoundObserver receives event → plays death sound
 * 5. World detects death → stops movement, starts death timer
 * 6. After death animation → respawn() called
 */
void PacMan::die() { notify(GameEvent::PACMAN_DIED); }

/**
 * @brief Reset PacMan state after death animation completes
 *
 * Called by World::updateDeathAnimation() when timer expires.
 * Resets to initial state for next life attempt.
 */
void PacMan::respawn() {
    setPosition(spawnX, spawnY);
    setDirection(Direction::LEFT);
    desiredDirection = Direction::NONE;
    notify(GameEvent::DIRECTION_CHANGED);  // Reset view animation
}

void PacMan::setSpawnPosition(float x, float y) {
    spawnX = x;
    spawnY = y;
}

void PacMan::notifyDirectionChange() { notify(GameEvent::DIRECTION_CHANGED); }

void PacMan::hitWall() { notify(GameEvent::WALL_HIT); }

/**
 * @brief Check grid alignment for smooth turning
 *
 * PacMan can only turn when near a grid cell center to prevent
 * diagonal movement and ensure clean 90-degree turns.
 *
 * Tolerance: 0.1 grid units from center (e.g., position 10.4-10.6 is aligned)
 *
 * @return true if within tolerance of grid center on both axes
 */
bool PacMan::isAlignedWithGrid() const {
    auto [x, y] = getPosition();
    constexpr float tolerance = 0.1f;

    // Calculate fractional part (e.g., 10.3 → 0.3)
    float fractX = x - std::floor(x);
    float fractY = y - std::floor(y);

    // Check if near 0.0, 0.5, or 1.0 (grid boundaries and centers)
    return (std::abs(fractX - 0.5f) < tolerance || std::abs(fractX) < tolerance ||
            std::abs(fractX - 1.0f) < tolerance) &&
           (std::abs(fractY - 0.5f) < tolerance || std::abs(fractY) < tolerance ||
            std::abs(fractY - 1.0f) < tolerance);
}

/**
 * @brief Update PacMan position based on direction and speed
 *
 * Movement calculation:
 * - Delta = speed * deltaTime (e.g., 4.0 * 0.016 = 0.064 grid units)
 * - New position = current + delta in direction vector
 *
 * @param deltaTime Time since last frame (typically ~0.016s for 60 FPS)
 * @param canMove False during death animation, ready state, or level cleared
 *
 * @note Collision detection and direction changes handled by World
 */
void PacMan::update(float deltaTime, bool canMove) {
    auto [x, y] = getPosition();

    if (canMove && getDirection() != Direction::NONE) {
        // Apply velocity based on direction
        switch (getDirection()) {
        case Direction::UP:
            y -= speed * deltaTime;
            break;
        case Direction::DOWN:
            y += speed * deltaTime;
            break;
        case Direction::LEFT:
            x -= speed * deltaTime;
            break;
        case Direction::RIGHT:
            x += speed * deltaTime;
            break;
        case Direction::NONE:
            break;
        }

        setPosition(x, y);
    }
}

} // namespace logic