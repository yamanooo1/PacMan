//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_FRUITVIEW_H
#define PACMAN_FRUITVIEW_H
#include "EntityView.h"
#include <SFML/Graphics.hpp>

class FruitView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::CircleShape shape;
public:
  FruitView(EntityModel* model, sf::RenderWindow* win, Camera* cam);
  void update(GameEvent) override;
  void draw() override;
};


#endif //PACMAN_FRUITVIEW_H