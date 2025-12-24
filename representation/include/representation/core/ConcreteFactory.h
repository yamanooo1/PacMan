#ifndef PACMAN_CONCRETEFACTORY_H
#define PACMAN_CONCRETEFACTORY_H

#include "../../../../logic/include/logic/core/AbstractFactory.h"
#include "SFML/Graphics.hpp"
#include <memory>
#include <vector>

namespace logic {
class Observer;
}

namespace representation {

class EntityView;
class Camera;
class SpriteAtlas;

/**
 * @brief ConcreteFactory - creates paired model+view entities
 *
 * DESIGN PATTERN: Concrete Factory (implementation of AbstractFactory)
 *
 * Purpose:
 * - Bridge between logic layer (models) and representation layer (views)
 * - Creates entity pairs: logic::EntityModel + representation::EntityView
 * - Maintains view list for rendering and cleanup
 * - Attaches observers (views, sound) to models
 *
 * Architecture:
 * ```
 *        World (logic)
 *           ↓ calls
 *    AbstractFactory interface
 *           ↓ implements
 *    ConcreteFactory (representation)
 *           ↓ creates
 *    ┌──────────────────┐
 *    │ EntityModel      │  (returned to World)
 *    │  ↓ observed by   │
 *    │ EntityView       │  (stored in factory)
 *    └──────────────────┘
 * ```
 *
 * Creation Flow Example (PacMan):
 * ```
 * World::createPacMan()
 *   → factory->createPacMan(x, y)
 *     → new logic::PacMan (model)
 *     → new PacManView(model, window, camera) (view)
 *     → model->attach(view) (observer pattern)
 *     → store view in views vector
 *     → return unique_ptr<PacMan> to World
 * ```
 *
 * View Lifecycle Management:
 * - Views stored as shared_ptr (multiple references possible)
 * - Models notify views via Observer pattern
 * - Dead entities: removeDeadViews() cleans up matching views
 * - Views hold raw pointer to model (model owns lifetime)
 *
 * Rendering Coordination:
 * - drawAll(): iterates views and calls view->draw()
 * - updateAll(): animates all views (deltaTime updates)
 * - Separates rendering from game logic updates
 *
 * Observer Attachment:
 * - Views: Always attached to their model (for visual updates)
 * - SoundObserver: Attached for audio feedback on events
 * - Score/Lives: Attached by World (not factory's responsibility)
 *
 * @see logic::AbstractFactory for interface definition
 * @see EntityView for view base class
 */
class ConcreteFactory : public logic::AbstractFactory {
private:
    sf::RenderWindow& window;                       // SFML rendering target
    std::shared_ptr<Camera> camera;                 // Grid-to-screen transform
    std::shared_ptr<SpriteAtlas> spriteAtlas;       // Sprite sheet manager
    std::vector<std::shared_ptr<EntityView>> views; // All active entity views
    std::shared_ptr<logic::Observer> soundObserver; // Audio event handler

public:
    /**
     * @brief Initialize factory with rendering dependencies
     *
     * @param win SFML window for rendering
     * @param cam Camera for coordinate transformation
     */
    explicit ConcreteFactory(sf::RenderWindow& win, std::shared_ptr<Camera> cam);
    ~ConcreteFactory() override;

    /**
     * @brief Load sprite sheet texture into atlas
     *
     * Must be called before creating any entities.
     *
     * @param filepath Path to sprite sheet (e.g., "sprites/spritemap.png")
     * @return true if loading successful
     */
    bool loadSprites(const std::string& filepath);

    /**
     * @brief Set sound observer for audio events
     *
     * Attached to entities that trigger sounds (PacMan, Ghosts, Coins).
     *
     * @param observer SoundObserver instance
     */
    void setSoundObserver(const std::shared_ptr<logic::Observer>& observer) { soundObserver = observer; }

    // AbstractFactory interface implementation
    // Each creates model+view pair, attaches observers, returns model
    std::unique_ptr<logic::PacMan> createPacMan(float x, float y) override;
    std::unique_ptr<logic::Ghost> createGhost(float x, float y, logic::GhostType type, logic::GhostColor color,
                                              float waitTime, float speedMultiplier) override;
    std::unique_ptr<logic::Wall> createWall(float x, float y, float w, float h) override;
    std::unique_ptr<logic::Coin> createCoin(float x, float y) override;
    std::unique_ptr<logic::Fruit> createFruit(float x, float y) override;

    /**
     * @brief Remove views for dead entities (two-phase cleanup)
     *
     * Called by World::removeDeadEntities() before erasing models.
     * Prevents dangling view pointers.
     */
    void removeDeadViews() override;

    /**
     * @brief Render all entity views to window
     *
     * Called once per frame in LevelState::render().
     */
    void drawAll();

    /**
     * @brief Update all view animations
     *
     * Updates sprite frame timers for smooth animation.
     * Called once per frame in LevelState::render().
     */
    void updateAll();
};

} // namespace representation

#endif