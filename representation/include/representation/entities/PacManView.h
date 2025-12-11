#ifndef PACMAN_PACMANVIEW_H
#define PACMAN_PACMANVIEW_H

#include "../core/SpriteAtlas.h"
#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

class PacManView: public EntityView {
private:
  sf::RectangleShape shape;

  float animationTimer;
  int currentMouthFrame;
  float frameDuration;

  bool playingDeathAnimation;
  int deathFrame;
  float deathAnimationTimer;
  float deathFrameDuration;

public:
  PacManView(EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
             std::shared_ptr<SpriteAtlas> atlas);

  void update(GameEvent event) override;
  void updateAnimation(float deltaTime) override;
  void draw() override;
};

#endif