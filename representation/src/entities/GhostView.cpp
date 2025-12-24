#include <utility>

#include "../../../logic/include/logic/core/World.h"
#include "../../../logic/include/logic/entities/Ghost.h"
#include "../../include/representation/core/Camera.h"
#include "../../include/representation/core/SpriteAtlas.h"
#include "../../include/representation/entities/GhostView.h"

namespace representation {

using logic::Ghost;
using logic::Direction;
using logic::GameEvent;

GhostView::GhostView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                     std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, win, std::move(cam), std::move(atlas)), spriteType(SpriteGhostType::RED), animationTimer(0.0f),
      currentFrame(0), frameDuration(0.15f), flashTimer(0.0f), showWhite(false) {
    shape.setRadius(20.f);
    shape.setFillColor(sf::Color::Red);
}

void GhostView::update(logic::GameEvent event) {}

void GhostView::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;

    if (animationTimer >= frameDuration) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % 2;
    }

    logic::Ghost* ghost = static_cast<logic::Ghost*>(model);
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
    if (!model || !camera)
        return;

    auto [x, y] = model->getPosition();
    float centerX = x + model->getWidth() / 2.0f;
    float centerY = y + model->getHeight() / 2.0f;
    float screenX = camera->gridToScreenX(centerX);
    float screenY = camera->gridToScreenY(centerY);

    logic::Ghost* ghost = static_cast<logic::Ghost*>(model);
    bool shouldShowFear = (ghost && ghost->shouldShowFearMode());

    if (spriteAtlas) {
        std::shared_ptr<sf::Texture> texture = spriteAtlas->getTexture();

        if (texture && texture->getSize().x > 0) {
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
                logic::Direction dir = model->getDirection();
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

                window.draw(sprite);
                return;
            }
        }
    }

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

    window.draw(shape);
    shape.setFillColor(originalColor);
}

} // namespace representation