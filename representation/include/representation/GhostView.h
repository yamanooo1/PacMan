//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_GHOSTVIEW_H
#define PACMAN_GHOSTVIEW_H
#include "EntityView.h"
#include <SFML/Graphics.hpp>

class GhostView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::CircleShape shape;

public:

  GhostView(EntityModel* model, sf::RenderWindow* win, Camera* cam);
  void update(GameEvent) override;
  void draw() override;

  // NEW: Allow factory to set color
  void setColor(const sf::Color& color) { shape.setFillColor(color); }
};


#endif //PACMAN_GHOSTVIEW_H