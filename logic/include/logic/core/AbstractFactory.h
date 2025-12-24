#ifndef PACMAN_ABSTRACTFACTORY_H
#define PACMAN_ABSTRACTFACTORY_H
#include <memory>

namespace logic {

class PacMan;
class Ghost;
class Wall;
class Coin;
class Fruit;

enum class GhostType;
enum class GhostColor;

/**
 * @brief Abstract factory interface - bridges logic and representation layers
 *
 * Design Pattern: Abstract Factory
 * - Defines interface for creating entity families (models + views)
 * - Decouples logic layer from representation layer
 * - Enables different implementations (e.g., testing vs rendering)
 *
 * Architecture:
 * ```
 * logic::World (logic layer)
 *      ↓ uses
 * logic::AbstractFactory (interface)
 *      ↓ implemented by
 * representation::ConcreteFactory (representation layer)
 *      ↓ creates
 * logic::EntityModel + representation::EntityView
 * ```
 *
 * Responsibility:
 * - Create entity models (logic::PacMan, logic::Ghost, etc.)
 * - Create and attach entity views (representation::PacManView, etc.)
 * - Manage view lifecycle (cleanup dead views)
 *
 * Why Abstract Factory?
 * - logic:: namespace can't directly create representation:: objects
 * - Factory injection allows World to create entities without SFML dependencies
 * - Testability: can inject mock factory for unit tests
 *
 * Implementation:
 * - ConcreteFactory: Real implementation with SFML views
 * - MockFactory (potential): Test implementation with no rendering
 *
 * Usage Flow:
 * 1. LevelState creates ConcreteFactory
 * 2. LevelState creates World with factory pointer
 * 3. World calls factory->createPacMan(x, y)
 * 4. ConcreteFactory creates model + view, attaches observers
 * 5. ConcreteFactory returns unique_ptr<PacMan> (model only)
 * 6. World stores model, factory stores view
 *
 * @see representation::ConcreteFactory for concrete implementation
 */
class AbstractFactory {
public:
    virtual ~AbstractFactory() = default;

    /**
     * @brief Create PacMan entity (model + view)
     *
     * @param x Grid X position
     * @param y Grid Y position
     * @return unique_ptr to PacMan model (view stored internally)
     *
     * Implementation (ConcreteFactory):
     * 1. Create logic::PacMan model
     * 2. Create representation::PacManView with model reference
     * 3. Attach view as observer to model
     * 4. Store view in internal list
     * 5. Return model to World
     */
    virtual std::unique_ptr<PacMan> createPacMan(float x, float y) = 0;

    /**
     * @brief Create Ghost entity (model + view)
     *
     * @param x Grid X position
     * @param y Grid Y position
     * @param type AI personality (RANDOM/CHASER/AMBUSHER)
     * @param color Visual identifier (RED/PINK/CYAN/ORANGE)
     * @param waitTime Seconds to wait in spawn before exiting
     * @param speedMultiplier Level difficulty scaling
     * @return unique_ptr to Ghost model
     */
    virtual std::unique_ptr<Ghost> createGhost(float x, float y, GhostType type, GhostColor color, float waitTime,
                                               float speedMultiplier) = 0;

    /**
     * @brief Create Wall entity (model + view)
     *
     * @param x Grid X position
     * @param y Grid Y position
     * @param w Width (typically 1.0)
     * @param h Height (typically 1.0)
     * @return unique_ptr to Wall model
     */
    virtual std::unique_ptr<Wall> createWall(float x, float y, float w, float h) = 0;

    /**
     * @brief Create Coin entity (model + view)
     *
     * @param x Grid X position
     * @param y Grid Y position
     * @return unique_ptr to Coin model
     */
    virtual std::unique_ptr<Coin> createCoin(float x, float y) = 0;

    /**
     * @brief Create Fruit entity (model + view)
     *
     * @param x Grid X position
     * @param y Grid Y position
     * @return unique_ptr to Fruit model
     */
    virtual std::unique_ptr<Fruit> createFruit(float x, float y) = 0;

    /**
     * @brief Remove views for dead entities
     *
     * Called by World::removeDeadEntities() before removing models.
     * Cleanup sequence:
     * 1. World calls factory->removeDeadViews()
     * 2. Factory iterates views, checks if model->isDead()
     * 3. Remove dead views from internal list
     * 4. World removes dead models from entity list
     *
     * This ensures views are cleaned up before models are destroyed.
     */
    virtual void removeDeadViews() = 0;
};

} // namespace logic

#endif