//
// WallView.h - UPDATED constructor
//

#ifndef PACMAN_WALLVIEW_H
#define PACMAN_WALLVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

class WallView: public EntityView {
private:
  sf::RenderWindow* window;
  sf::RectangleShape shape;

public:
  WallView(EntityModel* model, sf::RenderWindow* win, std::shared_ptr<Camera> cam,
           std::shared_ptr<SpriteAtlas> atlas);  // ✅ UPDATED

  void update(GameEvent) override;
  void draw() override;
};

#endif //PACMAN_WALLVIEW_H