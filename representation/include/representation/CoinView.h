//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_COINVIEW_H
#define PACMAN_COINVIEW_H
#include "EntityView.h"
#include <SFML/Graphics.hpp>

class CoinView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::CircleShape shape;
public:

  CoinView(EntityModel* model, sf::RenderWindow* win);
  void update(GameEvent) override;
  void draw() override;

};


#endif //PACMAN_COINVIEW_H