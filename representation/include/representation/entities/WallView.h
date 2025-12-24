#ifndef PACMAN_WALLVIEW_H
#define PACMAN_WALLVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

/**
 * @brief WallView - renders maze walls
 *
 * Characteristics:
 * - Static (no animation)
 * - Large: Full grid cell (1.0×1.0)
 * - Blue color
 * - No collision response needed (logic handles)
 *
 * Rendering:
 * - Blue rectangles (no sprite - procedural)
 * - Fills entire grid cell
 */
class WallView : public EntityView {
private:
    sf::RectangleShape shape; // Blue rectangle

public:
    WallView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
             std::shared_ptr<SpriteAtlas> atlas);

    void update(logic::GameEvent) override;
    void draw() override;
};

} // namespace representation

#endif