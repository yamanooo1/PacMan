#include <utility>

#include "../../../logic/include/logic/core/World.h"
#include "../../../logic/include/logic/entities/Ghost.h"
#include "../../include/representation/core/Camera.h"
#include "../../include/representation/core/SpriteAtlas.h"
#include "../../include/representation/entities/GhostView.h"

namespace representation {

using logic::Direction;
using logic::GameEvent;
using logic::Ghost;

/**
 * @brief Initialize GhostView with default red appearance
 *
 * ConcreteFactory will call setColor() and setSpriteType()
 * to configure actual ghost appearance.
 */
GhostView::GhostView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                     std::shared_ptr<SpriteAtlas> atlas, int z)
    : EntityView(model, win, std::move(cam), std::move(atlas), z), spriteType(SpriteGhostType::RED),
      animationTimer(0.0f), currentFrame(0), frameDuration(0.15f), flashTimer(0.0f), showWhite(false) {
    shape.setRadius(20.f);
    shape.setFillColor(sf::Color::Red);
}

void GhostView::update(logic::GameEvent event) {}

/**
 * @brief Update ghost animation and fear flashing
 *
 * Frame Animation (always running):
 * - Toggle between frame 0 and 1 every 0.15s
 * - Simple walking animation
 *
 * Fear Flashing (conditional):
 * - Only when in fear mode AND ending phase
 * - Toggle showWhite flag every 0.2s
 * - Provides visual warning before vulnerability ends
 */
void GhostView::updateAnimation(float deltaTime) {
    // Frame animation (2-frame cycle)
    animationTimer += deltaTime;

    if (animationTimer >= frameDuration) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % 2;
    }

    // Fear mode flashing (ending phase only)
    logic::Ghost* ghost = static_cast<logic::Ghost*>(model);
    if (ghost && ghost->shouldShowFearMode() && ghost->isFearModeEnding()) {
        flashTimer += deltaTime;

        if (flashTimer >= 0.2f) {
            flashTimer = 0.0f;
            showWhite = !showWhite; // Toggle blue ↔ white
        }
    } else {
        // Reset flash state when not in ending phase
        showWhite = false;
        flashTimer = 0.0f;
    }
}

/**
 * @brief Render ghost with fear mode consideration
 *
 * Fear Mode Detection:
 * - Cast model to Ghost* to access fear methods
 * - shouldShowFearMode(): Returns true if vulnerable
 * - Overrides normal color sprites with blue/white
 *
 * Sprite Selection Priority:
 * 1. Fear mode (ending): Blue or white (flashing)
 * 2. Fear mode (solid): Blue only
 * 3. Normal: Color-specific directional sprite
 *
 * Scaling:
 * - Ghosts render at 80% of grid cell size
 * - Centered on entity position
 */
void GhostView::draw() {
    if (!model || !camera)
        return;

    // Get center position for rendering
    auto [x, y] = model->getPosition();
    float centerX = x + model->getWidth() / 2.0f;
    float centerY = y + model->getHeight() / 2.0f;
    float screenX = camera->gridToScreenX(centerX);
    float screenY = camera->gridToScreenY(centerY);

    // Check fear mode state
    logic::Ghost* ghost = static_cast<logic::Ghost*>(model);
    bool shouldShowFear = (ghost && ghost->shouldShowFearMode());

    if (spriteAtlas) {
        std::shared_ptr<sf::Texture> texture = spriteAtlas->getTexture();

        if (texture && texture->getSize().x > 0) {
            sf::Sprite sprite;
            sprite.setTexture(*texture);

            sf::IntRect rect;

            if (shouldShowFear) {
                // Fear mode sprite selection
                int fearFrame = currentFrame;
                if (showWhite) {
                    fearFrame += 2; // Offset to white frames (2-3)
                }

                rect = spriteAtlas->getFearSprite(fearFrame);
            } else {
                // Normal colored sprite
                logic::Direction dir = model->getDirection();
                GhostFrame frame = (currentFrame == 0) ? GhostFrame::FRAME_1 : GhostFrame::FRAME_2;
                rect = spriteAtlas->getGhostSprite(spriteType, dir, frame);
            }

            sprite.setTextureRect(rect);

            float spriteWidth = static_cast<float>(rect.width);
            float spriteHeight = static_cast<float>(rect.height);

            if (spriteWidth > 0 && spriteHeight > 0) {
                // Scale to 80% of grid cell
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

    // Fallback: colored circle
    float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
    float desiredSize = gridCellSize * 0.8f;
    float radius = desiredSize / 2.f;

    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(screenX, screenY);

    // Change color to blue if in fear mode
    sf::Color originalColor = shape.getFillColor();
    if (shouldShowFear) {
        shape.setFillColor(sf::Color::Blue);
    }

    window.draw(shape);
    shape.setFillColor(originalColor); // Restore color
}

} // namespace representation