//
// FruitView.cpp - UPDATED with window reference
//

#include "representation/FruitView.h"
#include "representation/Camera.h"
#include "representation/SpriteAtlas.h"
#include <iostream>

FruitView::FruitView(EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,  // ✅ Reference
                     std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, win, cam, atlas)  // ✅ Pass to base
{
  shape.setRadius(8.f);
  shape.setFillColor(sf::Color::Green);
}

void FruitView::update(GameEvent event) {}

void FruitView::draw() {
  if (!model || !camera) return;
  if (model->isDead()) return;

  auto [x, y] = model->getPosition();
  float screenX = camera->gridToScreenX(x);
  float screenY = camera->gridToScreenY(y);

  if (spriteAtlas) {
    std::shared_ptr<sf::Texture> texture = spriteAtlas->getTexture();

    if (texture && texture->getSize().x > 0) {
      try {
        sf::Sprite sprite;
        sprite.setTexture(*texture);

        sf::IntRect rect = spriteAtlas->getFruitSprite();
        sprite.setTextureRect(rect);

        float spriteWidth = static_cast<float>(rect.width);
        float spriteHeight = static_cast<float>(rect.height);

        if (spriteWidth > 0 && spriteHeight > 0) {
          float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
          float desiredSize = gridCellSize * 0.7f;

          float scaleX = desiredSize / spriteWidth;
          float scaleY = desiredSize / spriteHeight;

          sprite.setScale(scaleX, scaleY);
          sprite.setOrigin(spriteWidth / 2.f, spriteHeight / 2.f);
          sprite.setPosition(screenX, screenY);

          window.draw(sprite);  // ✅ CHANGED
          return;
        }
      } catch (const std::exception& e) {
        std::cerr << "[FRUIT] Error drawing sprite: " << e.what() << std::endl;
      }
    }
  }

  // Fallback
  float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
  float desiredSize = gridCellSize * 0.5f;
  float radius = desiredSize / 2.f;

  shape.setRadius(radius);
  shape.setOrigin(radius, radius);
  shape.setPosition(screenX, screenY);

  window.draw(shape);  // ✅ CHANGED
}