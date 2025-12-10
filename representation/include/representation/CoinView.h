#ifndef PACMAN_COINVIEW_H
#define PACMAN_COINVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

class CoinView: public EntityView {
private:
  sf::CircleShape shape;

public:
  CoinView(EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
           std::shared_ptr<SpriteAtlas> atlas);

  void update(GameEvent) override;
  void draw() override;
};

#endif