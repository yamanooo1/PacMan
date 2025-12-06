//
// PacManView.cpp - FIXED with separate event and animation handling
//

#include "representation/PacManView.h"
#include "representation/Camera.h"
#include "representation/SpriteAtlas.h"
#include <iostream>

PacManView::PacManView(EntityModel* model, sf::RenderWindow* win, std::shared_ptr<Camera> cam,
                       std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, cam, atlas)
    , window(win)
    , animationTimer(0.0f)
    , currentMouthFrame(0)
    , frameDuration(0.1f)
    , playingDeathAnimation(false)
    , deathFrame(0)
    , deathAnimationTimer(0.0f)
    , deathFrameDuration(0.18f)
{
  shape.setFillColor(sf::Color::Yellow);
}

// ✅ Handle events ONLY (not called every frame)
void PacManView::update(GameEvent event) {
  switch (event) {
  case GameEvent::PACMAN_DIED:
    std::cout << "[VIEW] PacMan died - starting death animation" << std::endl;
    playingDeathAnimation = true;
    deathFrame = 0;
    deathAnimationTimer = 0.0f;
    currentMouthFrame = 0;
    animationTimer = 0.0f;
    break;

  case GameEvent::DIRECTION_CHANGED:
    // ✅ If death animation finished and PacMan moves, respawn happened
    if (playingDeathAnimation && deathFrame >= 10) {
      std::cout << "[VIEW] Detected respawn - resetting death animation" << std::endl;
      playingDeathAnimation = false;
      deathFrame = 0;
      deathAnimationTimer = 0.0f;
    } else if (!playingDeathAnimation) {
      std::cout << "[VIEW] PacMan changed direction" << std::endl;
    }
    break;

  default:
    break;
  }
}

// ✅ Update animation every frame (separate from events)
// ✅ Update animation every frame (separate from events)
void PacManView::updateAnimation(float deltaTime) {
  // Update death animation
  if (playingDeathAnimation) {
    deathAnimationTimer += deltaTime;

    if (deathAnimationTimer >= deathFrameDuration) {
      deathAnimationTimer = 0.0f;
      deathFrame++;

      // Death animation has 11 frames (0-10)
      if (deathFrame > 10) {
        // Stay on last frame until respawn detected via DIRECTION_CHANGED event
        deathFrame = 10;
      }
    }

    return;  // Don't update mouth animation during death
  }

  // ✅ FIX: Only animate if position actually changed (detect movement)
  if (model) {
    static float prevX = 0.0f;
    static float prevY = 0.0f;

    auto [currentX, currentY] = model->getPosition();
    bool actuallyMoving = (std::abs(currentX - prevX) > 0.001f ||
                           std::abs(currentY - prevY) > 0.001f);

    if (actuallyMoving && model->getDirection() != Direction::NONE) {
      // PacMan is moving - animate mouth
      animationTimer += deltaTime;

      if (animationTimer >= frameDuration) {
        animationTimer = 0.0f;
        currentMouthFrame = (currentMouthFrame + 1) % 3;
      }
    } else {
      // PacMan is stationary (ready state, stopped, etc.) - show closed mouth
      currentMouthFrame = 0;
    }

    // Update previous position for next frame
    prevX = currentX;
    prevY = currentY;
  } else {
    currentMouthFrame = 0;
  }
}

void PacManView::draw() {
  if (!model || !window || !camera) return;

  auto [x, y] = model->getPosition();
  float screenX = camera->gridToScreenX(x);
  float screenY = camera->gridToScreenY(y);

  if (spriteAtlas) {
    std::shared_ptr<sf::Texture> texture = spriteAtlas->getTexture();

    if (texture && texture->getSize().x > 0) {
      try {
        sf::Sprite sprite;
        sprite.setTexture(*texture);

        sf::IntRect rect;

        if (playingDeathAnimation) {
          DeathFrame frame = static_cast<DeathFrame>(deathFrame);
          rect = spriteAtlas->getDeathSprite(frame);
        } else {
          Direction dir = model->getDirection();

          PacManFrame frame;
          switch (currentMouthFrame) {
            case 0: frame = PacManFrame::CLOSED; break;
            case 1: frame = PacManFrame::HALF_OPEN; break;
            case 2: frame = PacManFrame::FULL_OPEN; break;
            default: frame = PacManFrame::CLOSED; break;
          }

          rect = spriteAtlas->getPacManSprite(dir, frame);
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

          float centerX = x + model->getWidth() / 2.0f;
          float centerY = y + model->getHeight() / 2.0f;
          float centerScreenX = camera->gridToScreenX(centerX);
          float centerScreenY = camera->gridToScreenY(centerY);

          sprite.setPosition(centerScreenX, centerScreenY);

          window->draw(sprite);
          return;
        }
      } catch (const std::exception& e) {
        std::cerr << "[PACMAN] Error drawing sprite: " << e.what() << std::endl;
      }
    }
  }

  float screenWidth = camera->gridToScreenX(model->getWidth());
  float screenHeight = camera->gridToScreenY(model->getHeight());

  shape.setSize(sf::Vector2f(screenWidth, screenHeight));
  shape.setPosition(screenX, screenY);

  window->draw(shape);
}