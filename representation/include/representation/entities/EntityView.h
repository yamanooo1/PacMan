#ifndef PACMAN_ENTITYVIEW_H
#define PACMAN_ENTITYVIEW_H

#include "../../../../logic/include/logic/core/EntityModel.h"
#include "../../../../logic/include/logic/patterns/Observer.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

class Camera;
class SpriteAtlas;

/**
 * @brief EntityView - abstract base class for entity rendering
 *
 * DESIGN PATTERN: Observer (observes logic::EntityModel for visual updates)
 *
 * Purpose:
 * - Bridge between logic layer (EntityModel) and rendering (SFML)
 * - Observes model for game events (death, collection, direction change)
 * - Handles sprite rendering and animation
 * - Manages coordinate transformation via Camera
 *
 * Architecture:
 * ```
 *    EntityModel (logic)
 *         ↓ notifies
 *    EntityView (representation) ←─ Observer pattern
 *         ↓ uses
 *    Camera + SpriteAtlas + SFML
 * ```
 *
 * Lifetime Management:
 * - View holds RAW pointer to model (model owns lifetime)
 * - Model notifies via weak_ptr (in Subject class)
 * - View deleted when model dies (ConcreteFactory cleanup)
 * - Never dereference model without checking if dead
 *
 * Rendering Flow:
 * ```
 * ConcreteFactory::drawAll()
 *   → for each view: view->draw()
 *     → if (!model || model->isDead()) return
 *     → auto [x, y] = model->getPosition()
 *     → screenX = camera->gridToScreenX(x)
 *     → sprite.setPosition(screenX, screenY)
 *     → window.draw(sprite)
 * ```
 *
 * Observer Events:
 * - PACMAN_DIED: Trigger death animation
 * - COIN_COLLECTED: Mark view for removal
 * - DIRECTION_CHANGED: Reset animation state
 * - GHOST_EATEN: Visual feedback (flashing, etc.)
 *
 * Animation System:
 * - updateAnimation(deltaTime): Called once per frame
 * - Increments frame timers (animationTimer += deltaTime)
 * - Switches sprite frames when timer exceeds threshold
 * - Different timing per entity (PacMan: 0.1s, Ghost: 0.15s)
 *
 * Coordinate Transformation:
 * ```cpp
 * // Grid space (logic):
 * auto [gridX, gridY] = model->getPosition();  // e.g., (10.5, 5.3)
 *
 * // Screen space (pixels):
 * float screenX = camera->gridToScreenX(gridX);  // → 399px
 * float screenY = camera->gridToScreenY(gridY);  // → 201px
 *
 * sprite.setPosition(screenX, screenY);
 * ```
 *
 * Sprite Rendering Fallback:
 * - Primary: Use SpriteAtlas texture + IntRect
 * - Fallback: Use SFML shapes (circles, rectangles) if sprites fail
 * - Allows debugging without sprite sheet
 *
 * @see logic::Observer for observer pattern interface
 * @see ConcreteFactory for view creation
 */
class EntityView : public logic::Observer {
public:
    /**
     * @brief Initialize view with model and rendering dependencies
     *
     * @param m Model to observe (raw pointer, model owns lifetime)
     * @param win SFML window for rendering
     * @param cam Camera for grid-to-screen transformation
     * @param atlas SpriteAtlas for texture/sprite lookup
     * @param z Z-order for rendering (lower = background, higher = foreground)
     *          Default values: Walls=0, Coins/Fruits=10, PacMan/Ghosts=100
     */
    explicit EntityView(logic::EntityModel* m, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                        std::shared_ptr<SpriteAtlas> atlas, int z = 0)
        : model(m), window(win), camera(std::move(cam)), spriteAtlas(std::move(atlas)), zOrder(z) {}

    ~EntityView() override = default;

    /**
     * @brief Handle game events from model (Observer pattern)
     *
     * Default: no-op (subclasses override for specific events)
     *
     * @param event Game event type (COIN_COLLECTED, PACMAN_DIED, etc.)
     */
    void update(logic::GameEvent event) override {}

    /**
     * @brief Update animation frame timers (called once per frame)
     *
     * Default: no-op (subclasses override for animated entities)
     *
     * @param deltaTime Time since last frame in seconds
     */
    virtual void updateAnimation(float deltaTime) {}

    /**
     * @brief Render entity to window (pure virtual - must override)
     *
     * Typical implementation:
     * 1. Check if model valid and alive
     * 2. Get grid position from model
     * 3. Transform to screen coordinates
     * 4. Set sprite texture/rect
     * 5. Draw sprite/shape to window
     */
    virtual void draw() = 0;

    // Prevent copying (views tied to specific model instance)
    EntityView(const EntityView&) = delete;
    EntityView& operator=(const EntityView&) = delete;

    /**
     * @brief Get observed model pointer
     *
     * Used by ConcreteFactory for dead entity cleanup.
     *
     * @return Raw pointer to model
     */
    [[nodiscard]] logic::EntityModel* getModel() const { return model; }

    /**
     * @brief Get z-order for rendering depth sorting
     *
     * Lower values render first (background), higher values last (foreground).
     * Used by ConcreteFactory::drawAll() to sort views before rendering.
     *
     * @return Z-order value (0=background, 100=foreground)
     */
    [[nodiscard]] int getZOrder() const { return zOrder; }

protected:
    logic::EntityModel* model;                  // Observed model (raw pointer)
    sf::RenderWindow& window;                   // SFML rendering target
    std::shared_ptr<Camera> camera;             // Coordinate transformer
    std::shared_ptr<SpriteAtlas> spriteAtlas;   // Sprite sheet manager
    int zOrder;                                 // Render depth (0=back, 100=front)
};

} // namespace representation

#endif