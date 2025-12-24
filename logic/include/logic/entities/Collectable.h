#ifndef PACMAN_COLLECTABLES_H
#define PACMAN_COLLECTABLES_H
#include "../core/EntityModel.h"

namespace logic {

/**
 * @brief Base class for collectible items (coins and fruits)
 *
 * Collectables are entities that PacMan can pick up for points.
 * They share common collision behavior: notify observers and mark as dead.
 *
 * Derived classes:
 * - Coin: Small pellets, 10-30 points (decay system)
 * - Fruit: Power pellets, 50 points + fear mode activation
 *
 * Observer Pattern:
 * - Notifies specific collection events (COIN_COLLECTED, FRUIT_COLLECTED)
 * - Score system observes and updates points
 * - SoundObserver plays collection sound effects
 *
 * Lifecycle:
 * - Created at map load from '.' (coin) or 'f' (fruit) markers
 * - Exists until collected
 * - Marked dead on collection → removed by World::removeDeadEntities()
 *
 * Size Convention:
 * - Coin: 0.02x0.02 (tiny dots)
 * - Fruit: 0.03x0.03 (larger power pellets)
 * - Used by collision detection to distinguish types
 */
class Collectable : public EntityModel {
public:
    /**
     * @brief Construct a collectable item
     *
     * @param x Grid X position
     * @param y Grid Y position
     * @param width Item width (0.02 for coin, 0.03 for fruit)
     * @param height Item height
     */
    Collectable(float x, float y, float width, float height);
    ~Collectable() override = default;

    /**
     * @brief Handle collection event (pure virtual)
     *
     * Called by onCollisionWithPacMan().
     * Derived classes implement to notify specific event type.
     *
     * Implementation:
     * - Coin: notify(COIN_COLLECTED)
     * - Fruit: notify(FRUIT_COLLECTED)
     */
    virtual void onCollected() = 0;

    /**
     * @brief Handle collision with PacMan
     *
     * Overrides EntityModel::onCollisionWithPacMan().
     * Template method pattern:
     * 1. Call derived class's onCollected() (specific event)
     * 2. Mark self as dead (common cleanup)
     */
    void onCollisionWithPacMan() override;
};

} // namespace logic

#endif