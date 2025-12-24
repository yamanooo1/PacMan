#include <utility>

#include "../../include/representation/core/Camera.h"
#include "../../include/representation/core/SpriteAtlas.h"
#include "../../include/representation/entities/PacManView.h"

namespace representation {

using logic::Direction;
using logic::GameEvent;

PacManView::PacManView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                       std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, win, std::move(cam), std::move(atlas)), animationTimer(0.0f), currentMouthFrame(0),
      frameDuration(0.1f), playingDeathAnimation(false), deathFrame(0), deathAnimationTimer(0.0f),
      deathFrameDuration(0.18f) {
    shape.setFillColor(sf::Color::Yellow);
}

void PacManView::update(logic::GameEvent event) {
    switch (event) {
    case logic::GameEvent::PACMAN_DIED:
        playingDeathAnimation = true;
        deathFrame = 0;
        deathAnimationTimer = 0.0f;
        currentMouthFrame = 0;
        animationTimer = 0.0f;
        break;

    case logic::GameEvent::DIRECTION_CHANGED:
        if (playingDeathAnimation && deathFrame >= 10) {
            playingDeathAnimation = false;
            deathFrame = 0;
            deathAnimationTimer = 0.0f;
        }
        break;

    default:
        break;
    }
}

void PacManView::updateAnimation(float deltaTime) {
    if (playingDeathAnimation) {
        deathAnimationTimer += deltaTime;

        if (deathAnimationTimer >= deathFrameDuration) {
            deathAnimationTimer = 0.0f;
            deathFrame++;

            if (deathFrame > 10) {
                deathFrame = 10;
            }
        }

        return;
    }

    if (model) {
        static float prevX = 0.0f;
        static float prevY = 0.0f;

        auto [currentX, currentY] = model->getPosition();
        bool actuallyMoving = (std::abs(currentX - prevX) > 0.001f || std::abs(currentY - prevY) > 0.001f);

        if (actuallyMoving && model->getDirection() != logic::Direction::NONE) {
            animationTimer += deltaTime;

            if (animationTimer >= frameDuration) {
                animationTimer = 0.0f;
                currentMouthFrame = (currentMouthFrame + 1) % 3;
            }
        } else {
            currentMouthFrame = 0;
        }

        prevX = currentX;
        prevY = currentY;
    } else {
        currentMouthFrame = 0;
    }
}

void PacManView::draw() {
    if (!model || !camera)
        return;

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
                    auto frame = static_cast<DeathFrame>(deathFrame);
                    rect = spriteAtlas->getDeathSprite(frame);
                } else {
                    logic::Direction dir = model->getDirection();

                    PacManFrame frame;
                    switch (currentMouthFrame) {
                    case 0:
                        frame = PacManFrame::CLOSED;
                        break;
                    case 1:
                        frame = PacManFrame::HALF_OPEN;
                        break;
                    case 2:
                        frame = PacManFrame::FULL_OPEN;
                        break;
                    default:
                        frame = PacManFrame::CLOSED;
                        break;
                    }

                    rect = spriteAtlas->getPacManSprite(dir, frame);
                }

                sprite.setTextureRect(rect);

                auto spriteWidth = static_cast<float>(rect.width);
                auto spriteHeight = static_cast<float>(rect.height);

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

                    window.draw(sprite);
                    return;
                }
            } catch (const std::exception& e) {
            }
        }
    }

    float screenWidth = camera->gridToScreenX(model->getWidth());
    float screenHeight = camera->gridToScreenY(model->getHeight());

    shape.setSize(sf::Vector2f(screenWidth, screenHeight));
    shape.setPosition(screenX, screenY);

    window.draw(shape);
}

} // namespace representation