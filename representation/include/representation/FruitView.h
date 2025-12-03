//
// FruitView.h - UPDATED constructor
//

#ifndef PACMAN_FRUITVIEW_H
#define PACMAN_FRUITVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

class FruitView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::CircleShape shape;

public:
  FruitView(EntityModel* model, sf::RenderWindow* win, Camera* cam,
            std::shared_ptr<SpriteAtlas> atlas);  // ✅ UPDATED

  void update(GameEvent) override;
  void draw() override;
};

#endif //PACMAN_FRUITVIEW_H