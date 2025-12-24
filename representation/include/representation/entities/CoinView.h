#ifndef PACMAN_COINVIEW_H
#define PACMAN_COINVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

class CoinView : public EntityView {
private:
    sf::CircleShape shape;

public:
    CoinView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
             std::shared_ptr<SpriteAtlas> atlas);

    void update(logic::GameEvent) override;
    void draw() override;
};

} // namespace representation

#endif