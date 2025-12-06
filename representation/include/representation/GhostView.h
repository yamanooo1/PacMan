//
// GhostView.h - WITH ANIMATION SUPPORT
//

#ifndef PACMAN_GHOSTVIEW_H
#define PACMAN_GHOSTVIEW_H

#include "EntityView.h"
#include "representation/SpriteAtlas.h"
#include <SFML/Graphics.hpp>
#include <memory>

class GhostView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::CircleShape shape;
  SpriteGhostType spriteType;

  // Animation state
  float animationTimer;
  int currentFrame;  // 0 or 1 (for FRAME_1 or FRAME_2)
  float frameDuration;

  // ✅ ADD THESE
  float flashTimer;
  bool showWhite;

public:
  GhostView(EntityModel* model, sf::RenderWindow* win, std::shared_ptr<Camera> cam,
            std::shared_ptr<SpriteAtlas> atlas);

  void update(GameEvent) override;
  void updateAnimation(float deltaTime);
  void draw() override;

  void setColor(const sf::Color& color) { shape.setFillColor(color); }
  void setSpriteType(SpriteGhostType type) { spriteType = type; }
};

#endif //PACMAN_GHOSTVIEW_H