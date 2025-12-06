//
// PacManView.h - UPDATED with window reference
//

#ifndef PACMAN_PACMANVIEW_H
#define PACMAN_PACMANVIEW_H

#include "EntityView.h"
#include "representation/SpriteAtlas.h"
#include <SFML/Graphics.hpp>
#include <memory>

class PacManView: public EntityView {
private:
  // ✅ REMOVED: sf::RenderWindow* window; - now in base class
  sf::RectangleShape shape;

  float animationTimer;
  int currentMouthFrame;
  float frameDuration;

  bool playingDeathAnimation;
  int deathFrame;
  float deathAnimationTimer;
  float deathFrameDuration;

public:
  PacManView(EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,  // ✅ Reference
             std::shared_ptr<SpriteAtlas> atlas);

  void update(GameEvent event) override;
  void updateAnimation(float deltaTime);
  void draw() override;
};

#endif //PACMAN_PACMANVIEW_H