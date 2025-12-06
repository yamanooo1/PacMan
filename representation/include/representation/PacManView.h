//
// PacManView.h - FIXED with separate methods
//

#ifndef PACMAN_PACMANVIEW_H
#define PACMAN_PACMANVIEW_H

#include "EntityView.h"
#include "representation/SpriteAtlas.h"
#include <SFML/Graphics.hpp>
#include <memory>

class PacManView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::RectangleShape shape;

  float animationTimer;
  int currentMouthFrame;
  float frameDuration;

  bool playingDeathAnimation;
  int deathFrame;
  float deathAnimationTimer;
  float deathFrameDuration;

public:
  PacManView(EntityModel* model, sf::RenderWindow* win, std::shared_ptr<Camera> cam,
             std::shared_ptr<SpriteAtlas> atlas);

  void update(GameEvent event) override;  // ✅ Events only
  void updateAnimation(float deltaTime);  // ✅ Animation every frame
  void draw() override;
};

#endif //PACMAN_PACMANVIEW_H