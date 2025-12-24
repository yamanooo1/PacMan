#ifndef PACMAN_ENTITY_H
#define PACMAN_ENTITY_H
#include "../patterns/Subject.h"
#include <utility>

namespace logic {

/**
 * @brief Direction enumeration for entity movement
 *
 * Represents the four cardinal directions plus a NONE state for stationary entities.
 */
enum class Direction { UP, DOWN, LEFT, RIGHT, NONE };

/**
 * @brief Base model class for all game entities
 *
 * EntityModel is the foundation of the logic layer in the Model-View architecture.
 * It stores position, size, direction, and lifecycle state for all game objects.
 *
 * Design Pattern: Subject (Observer Pattern)
 * - Inherits from Subject to notify observers (Views) of state changes
 * - Views in the representation layer observe models for rendering
 *
 * Architecture:
 * - Pure logic class - no rendering or SFML dependencies
 * - Entities in representation:: namespace create and observe these models
 * - Abstract base for PacMan, Ghost, Wall, Coin, Fruit, etc.
 *
 * Key Responsibilities:
 * - Position and size management (grid-based coordinates)
 * - Movement direction tracking
 * - Lifecycle management (dead/alive state)
 * - Collision detection support via position/size accessors
 */
class EntityModel : public Subject {
private:
    float x, y;                  // Grid-based position (not pixels)
    const float width, height;   // Entity dimensions in grid units
    Direction direction;         // Current movement direction
    bool dead;                   // Lifecycle flag for garbage collection

public:
    /**
     * @brief Construct a new Entity Model
     *
     * @param posX Initial X position in grid coordinates
     * @param posY Initial Y position in grid coordinates
     * @param w Width in grid units (typically 0.8 for entities, 1.0 for walls)
     * @param h Height in grid units
     */
    EntityModel(float posX, float posY, float w, float h);
    ~EntityModel() override = default;

    // Position accessors
    [[nodiscard]] std::pair<float, float> getPosition() const { return {x, y}; }
    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    // Size accessors (const dimensions set at construction)
    [[nodiscard]] float getWidth() const { return width; }
    [[nodiscard]] float getHeight() const { return height; }

    // Direction management
    [[nodiscard]] Direction getDirection() const { return direction; }
    void setDirection(Direction dir) { direction = dir; }

    // Lifecycle management
    [[nodiscard]] bool isDead() const { return dead; }
    void markDead() { dead = true; }

    /**
     * @brief Handle collision with PacMan
     *
     * Override in derived classes to implement collision behavior.
     * Default implementation does nothing (e.g., for walls).
     * Collectables mark themselves dead, Ghosts trigger game events.
     */
    virtual void onCollisionWithPacMan();
};

} // namespace logic

#endif