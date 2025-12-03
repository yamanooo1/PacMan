//
// CoinView.h - UPDATED constructor
//

#ifndef PACMAN_COINVIEW_H
#define PACMAN_COINVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

class CoinView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::CircleShape shape;

public:
  CoinView(EntityModel* model, sf::RenderWindow* win, Camera* cam,
           std::shared_ptr<SpriteAtlas> atlas);  // ✅ UPDATED

  void update(GameEvent) override;
  void draw() override;
};

#endif //PACMAN_COINVIEW_H