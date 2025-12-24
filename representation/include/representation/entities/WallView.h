#ifndef PACMAN_WALLVIEW_H
#define PACMAN_WALLVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

class WallView : public EntityView {
private:
    sf::RectangleShape shape;

public:
    WallView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
             std::shared_ptr<SpriteAtlas> atlas);

    void update(logic::GameEvent) override;
    void draw() override;
};

} // namespace representation

#endif