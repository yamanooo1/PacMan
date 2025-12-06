#include "representation/GhostView.h"
#include "representation/Camera.h"
#include "representation/SpriteAtlas.h"
#include "logic/Ghost.h"
#include "logic/World.h"
#include "logic/Stopwatch.h"
#include <iostream>

GhostView::GhostView(EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,  // ✅ Reference
                     std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, win, cam, atlas)  // ✅ Pass to base
    , spriteType(SpriteGhostType::RED)
    , animationTimer(0.0f)
    , currentFrame(0)
    , frameDuration(0.15f)
    , flashTimer(0.0f)
    , showWhite(false)
{
  shape.setRadius(20.f);
  shape.setFillColor(sf::Color::Red);
}

void GhostView::update(GameEvent event) {}

void GhostView::updateAnimation(float deltaTime) {
  animationTimer += deltaTime;

  if (animationTimer >= frameDuration) {
    animationTimer = 0.0f;
    currentFrame = (currentFrame + 1) % 2;
  }

  Ghost* ghost = static_cast<Ghost*>(model);
  if (ghost && ghost->shouldShowFearMode() && ghost->isFearModeEnding()) {
    flashTimer += deltaTime;

    if (flashTimer >= 0.2f) {
      flashTimer = 0.0f;
      showWhite = !showWhite;
    }
  } else {
    showWhite = false;
    flashTimer = 0.0f;
  }
}

void GhostView::draw() {
  if (!model || !camera) return;  // ✅ window always valid

  auto [x, y] = model->getPosition();
  float centerX = x + model->getWidth() / 2.0f;
  float centerY = y + model->getHeight() / 2.0f;
  float screenX = camera->gridToScreenX(centerX);
  float screenY = camera->gridToScreenY(centerY);

  Ghost* ghost = static_cast<Ghost*>(model);
  bool shouldShowFear = (ghost && ghost->shouldShowFearMode());

  if (spriteAtlas) {
    std::shared_ptr<sf::Texture> texture = spriteAtlas->getTexture();

    if (texture && texture->getSize().x > 0) {
      try {
        sf::Sprite sprite;
        sprite.setTexture(*texture);

        sf::IntRect rect;

        if (shouldShowFear) {
          int fearFrame = currentFrame;
          if (showWhite) {
            fearFrame += 2;
          }

          rect = spriteAtlas->getFearSprite(fearFrame);
        } else {
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

          window.draw(sprite);  // ✅ CHANGED
          return;
        }
      } catch (const std::exception& e) {
        std::cerr << "[GHOST] Error drawing sprite: " << e.what() << std::endl;
      }
    }
  }

  // Fallback
  float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
  float desiredSize = gridCellSize * 0.8f;
  float radius = desiredSize / 2.f;

  shape.setRadius(radius);
  shape.setOrigin(radius, radius);
  shape.setPosition(screenX, screenY);

  sf::Color originalColor = shape.getFillColor();
  if (shouldShowFear) {
    shape.setFillColor(sf::Color::Blue);
  }

  window.draw(shape);  // ✅ CHANGED
  shape.setFillColor(originalColor);
}