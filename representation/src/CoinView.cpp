//
// CoinView.cpp - SAFER VERSION with error checking
//

#include "representation/CoinView.h"
#include "representation/Camera.h"
#include "representation/SpriteAtlas.h"
#include <iostream>

CoinView::CoinView(EntityModel* model, sf::RenderWindow* win, Camera* cam,
                   std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, cam, atlas)
    , window(win)
{
  shape.setRadius(5.f);
  shape.setFillColor(sf::Color::White);
}

void CoinView::update(GameEvent event) {}

void CoinView::draw() {
  if (!model || !window || !camera) return;
  if (model->isDead()) return;

  auto [x, y] = model->getPosition();
  float screenX = camera->gridToScreenX(x);
  float screenY = camera->gridToScreenY(y);

  // ✅ Try to use sprite with better error checking
  if (spriteAtlas) {
    std::shared_ptr<sf::Texture> texture = spriteAtlas->getTexture();

    if (texture && texture->getSize().x > 0) {
      try {
        sf::Sprite sprite;
        sprite.setTexture(*texture);

        sf::IntRect rect = spriteAtlas->getCoinSprite();
        sprite.setTextureRect(rect);

        // Get sprite size
        float spriteWidth = static_cast<float>(rect.width);
        float spriteHeight = static_cast<float>(rect.height);

        if (spriteWidth > 0 && spriteHeight > 0) {
          // Calculate scale
          float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
          float desiredSize = gridCellSize * 0.4f;

          float scaleX = desiredSize / spriteWidth;
          float scaleY = desiredSize / spriteHeight;

          sprite.setScale(scaleX, scaleY);
          sprite.setOrigin(spriteWidth / 2.f, spriteHeight / 2.f);
          sprite.setPosition(screenX, screenY);

          window->draw(sprite);
          return;  // Successfully drew sprite, exit
        }
      } catch (const std::exception& e) {
        std::cerr << "[COIN] Error drawing sprite: " << e.what() << std::endl;
      }
    }
  }

  // Fallback: use circle shape
  float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
  float desiredSize = gridCellSize * 0.3f;
  float radius = desiredSize / 2.f;

  shape.setRadius(radius);
  shape.setOrigin(radius, radius);
  shape.setPosition(screenX, screenY);

  window->draw(shape);
}