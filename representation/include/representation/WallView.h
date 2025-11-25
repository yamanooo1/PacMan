//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_WALLVIEW_H
#define PACMAN_WALLVIEW_H
#include "EntityView.h"
#include <SFML/Graphics.hpp>

class WallView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::RectangleShape shape;
public:
  WallView(EntityModel* model, sf::RenderWindow* win);
  void update(GameEvent) override;
  void draw() override;
};


#endif //PACMAN_WALLVIEW_H