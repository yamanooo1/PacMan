//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_PACMANVIEW_H
#define PACMAN_PACMANVIEW_H
#include "EntityView.h"
#include <SFML/Graphics.hpp>

class PacManView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::RectangleShape shape;
public:
  PacManView(EntityModel* model, sf::RenderWindow* win, Camera* cam);
  void update(GameEvent) override;
  void draw() override;
};


#endif //PACMAN_PACMANVIEW_H