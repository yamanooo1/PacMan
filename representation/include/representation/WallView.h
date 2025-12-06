//
// WallView.h - UPDATED with window reference
//

#ifndef PACMAN_WALLVIEW_H
#define PACMAN_WALLVIEW_H

#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

class WallView: public EntityView {
private:
  // ✅ REMOVED: sf::RenderWindow* window;
  sf::RectangleShape shape;

public:
  WallView(EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,  // ✅ Reference
           std::shared_ptr<SpriteAtlas> atlas);

  void update(GameEvent) override;
  void draw() override;
};

#endif //PACMAN_WALLVIEW_H