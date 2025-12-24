#ifndef PACMAN_FRUITVIEW_H
#define PACMAN_FRUITVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

/**
 * @brief FruitView - renders power pellets (larger, green)
 *
 * Characteristics:
 * - Static (no animation)
 * - Medium: 70% of grid cell size
 * - Green color (fallback)
 * - Triggers fear mode on collection
 *
 * Rendering:
 * - Primary: Fruit sprite (33×42px cherry)
 * - Fallback: Green circle
 */
class FruitView : public EntityView {
private:
    sf::CircleShape shape; // Fallback green circle

public:
    FruitView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
              std::shared_ptr<SpriteAtlas> atlas);

    void update(logic::GameEvent) override;
    void draw() override;
};

} // namespace representation

#endif