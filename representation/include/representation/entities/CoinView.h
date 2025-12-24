#ifndef PACMAN_COINVIEW_H
#define PACMAN_COINVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

/**
 * @brief CoinView - renders small white pellets
 *
 * Characteristics:
 * - Static (no animation)
 * - Small: 40% of grid cell size
 * - White color
 * - Disappears on collection (isDead check)
 *
 * Rendering:
 * - Primary: Coin sprite from atlas (16×16px)
 * - Fallback: White circle
 *
 * @see Coin (logic) for collection mechanics
 */
class CoinView : public EntityView {
private:
    sf::CircleShape shape; // Fallback white circle

public:
    CoinView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
             std::shared_ptr<SpriteAtlas> atlas);

    void update(logic::GameEvent) override;
    void draw() override;
};

} // namespace representation

#endif