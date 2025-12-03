//
// GhostView.cpp - WITH ANIMATION
//

#include "representation/GhostView.h"
#include "representation/Camera.h"
#include "representation/SpriteAtlas.h"
#include "logic/Ghost.h"
#include "logic/Stopwatch.h"
#include <iostream>

GhostView::GhostView(EntityModel* model, sf::RenderWindow* win, Camera* cam,
                     std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, cam, atlas)
    , window(win)
    , spriteType(SpriteGhostType::RED)
    , animationTimer(0.0f)
    , currentFrame(0)
    , frameDuration(0.15f)  // Switch frames every 0.15 seconds
{
  shape.setRadius(20.f);
  shape.setFillColor(sf::Color::Red);
}

void GhostView::update(GameEvent event) {
  // ✅ Update animation timer every frame
  Stopwatch& stopwatch = Stopwatch::getInstance();
  animationTimer += stopwatch.getDeltaTime();

  // Switch frames when timer exceeds duration
  if (animationTimer >= frameDuration) {
    animationTimer = 0.0f;
    currentFrame = (currentFrame + 1) % 2;  // Toggle between 0 and 1
  }
}

void GhostView::draw() {
  if (!model || !window || !camera) return;

  auto [x, y] = model->getPosition();
  float centerX = x + model->getWidth() / 2.0f;
  float centerY = y + model->getHeight() / 2.0f;
  float screenX = camera->gridToScreenX(centerX);
  float screenY = camera->gridToScreenY(centerY);

  // Get ghost state for fear mode
  Ghost* ghost = static_cast<Ghost*>(model);
  bool inFearMode = (ghost && ghost->isInFearMode());

  // ✅ Try to use sprite with animation
  if (spriteAtlas) {
    std::shared_ptr<sf::Texture> texture = spriteAtlas->getTexture();

    if (texture && texture->getSize().x > 0) {
      try {
        sf::Sprite sprite;
        sprite.setTexture(*texture);

        sf::IntRect rect;

        // Get appropriate sprite rect
        if (inFearMode) {
          // ✅ Animate fear mode sprites too (alternate between 0 and 1)
          rect = spriteAtlas->getFearSprite(currentFrame);
        } else {
          // ✅ Use animated ghost sprite
          Direction dir = model->getDirection();
          GhostFrame frame = (currentFrame == 0) ? GhostFrame::FRAME_1 : GhostFrame::FRAME_2;
          rect = spriteAtlas->getGhostSprite(spriteType, dir, frame);
        }

        sprite.setTextureRect(rect);

        float spriteWidth = static_cast<float>(rect.width);
        float spriteHeight = static_cast<float>(rect.height);

        if (spriteWidth > 0 && spriteHeight > 0) {
          float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
          float desiredSize = gridCellSize * 0.8f;

          float scaleX = desiredSize / spriteWidth;
          float scaleY = desiredSize / spriteHeight;

          sprite.setScale(scaleX, scaleY);
          sprite.setOrigin(spriteWidth / 2.f, spriteHeight / 2.f);
          sprite.setPosition(screenX, screenY);

          window->draw(sprite);
          return;  // Successfully drew sprite
        }
      } catch (const std::exception& e) {
        std::cerr << "[GHOST] Error drawing sprite: " << e.what() << std::endl;
      }
    }
  }

  // Fallback: use circle shape
  float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
  float desiredSize = gridCellSize * 0.8f;
  float radius = desiredSize / 2.f;

  shape.setRadius(radius);
  shape.setOrigin(radius, radius);
  shape.setPosition(screenX, screenY);

  // Change color for fear mode
  sf::Color originalColor = shape.getFillColor();
  if (inFearMode) {
    shape.setFillColor(sf::Color::Blue);
  }

  window->draw(shape);
  shape.setFillColor(originalColor);
}

void GhostView::updateAnimation(float deltaTime) {  // ✅ MOVED from update()
  // Update animation timer every frame
  animationTimer += deltaTime;

  if (animationTimer >= frameDuration) {
    animationTimer = 0.0f;
    currentFrame = (currentFrame + 1) % 2;
  }
}