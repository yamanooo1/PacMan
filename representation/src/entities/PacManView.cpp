#include <utility>

#include "../../include/representation/core/Camera.h"
#include "../../include/representation/core/SpriteAtlas.h"
#include "../../include/representation/entities/PacManView.h"

namespace representation {

using logic::Direction;
using logic::GameEvent;

/**
 * @brief Initialize PacManView with default animation state
 *
 * Initial state: closed mouth, no death animation
 */
PacManView::PacManView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                       std::shared_ptr<SpriteAtlas> atlas, int z)
    : EntityView(model, win, std::move(cam), std::move(atlas), z), animationTimer(0.0f), currentMouthFrame(0),
      frameDuration(0.1f), playingDeathAnimation(false), deathFrame(0), deathAnimationTimer(0.0f),
      deathFrameDuration(0.18f) {
    shape.setFillColor(sf::Color::Yellow);
}

/**
 * @brief Handle death and respawn events
 *
 * PACMAN_DIED:
 * - Start death animation (11 frames)
 * - Reset animation timers
 *
 * DIRECTION_CHANGED (after death complete):
 * - End death animation
 * - Reset to normal state
 */
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

/**
 * @brief Update animation based on state (death vs normal)
 *
 * Death Animation:
 * - Advance frame every 0.18s
 * - Cap at frame 10 (last frame)
 * - Blocks normal animation
 *
 * Normal Animation (movement-based):
 * - Track position change (prevX/Y vs current)
 * - Only animate if actually moving (>0.001 delta)
 * - Cycle through 3 mouth frames (0.1s each)
 * - Static = closed mouth (frame 0)
 */
void PacManView::updateAnimation(float deltaTime) {
    if (playingDeathAnimation) {
        deathAnimationTimer += deltaTime;

        if (deathAnimationTimer >= deathFrameDuration) {
            deathAnimationTimer = 0.0f;
            deathFrame++;

            if (deathFrame > 10) {
                deathFrame = 10;  // Hold on last frame
            }
        }

        return;
    }

    // Movement detection for mouth animation
    if (model) {
        static float prevX = 0.0f;
        static float prevY = 0.0f;

        auto [currentX, currentY] = model->getPosition();
        bool actuallyMoving = (std::abs(currentX - prevX) > 0.001f || std::abs(currentY - prevY) > 0.001f);

        if (actuallyMoving && model->getDirection() != logic::Direction::NONE) {
            animationTimer += deltaTime;

            if (animationTimer >= frameDuration) {
                animationTimer = 0.0f;
                currentMouthFrame = (currentMouthFrame + 1) % 3;  // Cycle 0→1→2→0
            }
        } else {
            currentMouthFrame = 0;  // Closed mouth when static
        }

        prevX = currentX;
        prevY = currentY;
    } else {
        currentMouthFrame = 0;
    }
}

/**
 * @brief Render PacMan sprite with proper scaling and centering
 *
 * Rendering Pipeline:
 * 1. Validate model and camera
 * 2. Get position and transform to screen coords
 * 3. Load texture and select sprite rect
 * 4. Calculate scale (fit to 80% of grid cell)
 * 5. Center sprite origin
 * 6. Position and draw sprite
 * 7. Fallback to rectangle if sprite fails
 *
 * Sprite Selection:
 * - Death: deathSprites[deathFrame]
 * - Normal: pacmanSprites[direction][mouthFrame]
 *
 * Scaling Formula:
 * - Cell size: min(scaleX, scaleY) in pixels
 * - Desired: 80% of cell size
 * - Scale: desired / spriteWidth
 */
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

                // Select sprite based on animation state
                if (playingDeathAnimation) {
                    auto frame = static_cast<DeathFrame>(deathFrame);
                    rect = spriteAtlas->getDeathSprite(frame);
                } else {
                    logic::Direction dir = model->getDirection();

                    // Map mouth frame to enum
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
                    // Calculate scale to fit 80% of grid cell
                    float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
                    float desiredSize = gridCellSize * 0.8f;

                    float scaleX = desiredSize / spriteWidth;
                    float scaleY = desiredSize / spriteHeight;

                    sprite.setScale(scaleX, scaleY);
                    sprite.setOrigin(spriteWidth / 2.f, spriteHeight / 2.f);

                    // Center on entity position
                    float centerX = x + model->getWidth() / 2.0f;
                    float centerY = y + model->getHeight() / 2.0f;
                    float centerScreenX = camera->gridToScreenX(centerX);
                    float centerScreenY = camera->gridToScreenY(centerY);

                    sprite.setPosition(centerScreenX, centerScreenY);

                    window.draw(sprite);
                    return;
                }
            } catch (const std::exception& e) {
                // Fall through to rectangle fallback
            }
        }
    }

    // Fallback: yellow rectangle
    float screenWidth = camera->gridToScreenX(model->getWidth());
    float screenHeight = camera->gridToScreenY(model->getHeight());

    shape.setSize(sf::Vector2f(screenWidth, screenHeight));
    shape.setPosition(screenX, screenY);

    window.draw(shape);
}

} // namespace representation