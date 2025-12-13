#ifndef PACMAN_FRUITVIEW_H
#define PACMAN_FRUITVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

class FruitView : public EntityView {
private:
    sf::CircleShape shape;

public:
    FruitView(EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
              std::shared_ptr<SpriteAtlas> atlas);

    void update(GameEvent) override;
    void draw() override;
};

#endif