#ifndef PACMAN_PACMANVIEW_H
#define PACMAN_PACMANVIEW_H

#include "../core/SpriteAtlas.h"
#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

/**
 * @brief PacManView - renders PacMan with mouth animation and death sequence
 *
 * Animation States:
 * 1. Normal: Mouth opening/closing (3 frames × 4 directions)
 * 2. Death: 11-frame death animation (spinning fade)
 *
 * Mouth Animation (Normal State):
 * ```
 * Frame 0: Closed (full circle)    ●
 * Frame 1: Half open               ◐
 * Frame 2: Full open               ◔
 * → Loop back to Frame 0
 * ```
 * Timing: 0.1s per frame (10 FPS animation)
 *
 * Movement Detection:
 * - Tracks previous position (prevX, prevY)
 * - Compares to current position (>0.001 difference = moving)
 * - Only animates mouth when actually moving
 * - Prevents animation when stuck at wall
 *
 * Death Animation:
 * - Triggered by PACMAN_DIED event
 * - 11 frames at 0.18s each (~2 seconds total)
 * - Overrides normal rendering until complete
 * - Resets on DIRECTION_CHANGED event (respawn)
 *
 * Sprite Selection Logic:
 * ```cpp
 * if (playingDeathAnimation) {
 *   sprite = deathSprite[deathFrame];  // 0-10
 * } else {
 *   Direction dir = model->getDirection();
 *   PacManFrame frame = currentMouthFrame;  // 0-2
 *   sprite = pacmanSprite[dir][frame];
 * }
 * ```
 *
 * Fallback Rendering:
 * - Primary: Sprite from SpriteAtlas
 * - Fallback: Yellow rectangle (if sprites fail to load)
 *
 * @see EntityView for base rendering interface
 * @see SpriteAtlas for sprite frame definitions
 */
class PacManView : public EntityView {
private:
    sf::RectangleShape shape; // Fallback yellow rectangle

    // Normal animation state
    float animationTimer;  // Accumulates deltaTime
    int currentMouthFrame; // 0-2: closed, half, full open
    float frameDuration;   // 0.1s between frames

    // Death animation state
    bool playingDeathAnimation;
    int deathFrame; // 0-10: death animation progress
    float deathAnimationTimer;
    float deathFrameDuration; // 0.18s per death frame

public:
    /**
     * @brief Initialize PacManView with yellow fallback color
     *
     * @param model PacMan model to render
     * @param win SFML window
     * @param cam Camera for transformation
     * @param atlas Sprite atlas for textures
     */
    PacManView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
               std::shared_ptr<SpriteAtlas> atlas);

    /**
     * @brief Handle game events (death, respawn)
     *
     * Events:
     * - PACMAN_DIED: Start death animation
     * - DIRECTION_CHANGED: End death animation (respawn)
     *
     * @param event Game event type
     */
    void update(logic::GameEvent event) override;

    /**
     * @brief Update animation timers (called once per frame)
     *
     * Death Animation Priority:
     * - If playing death: advance death frames
     * - Else: check movement and animate mouth
     *
     * Movement-Based Animation:
     * - Compare current vs previous position
     * - Only animate if position changed > 0.001
     * - Static PacMan shows closed mouth
     *
     * @param deltaTime Time since last frame
     */
    void updateAnimation(float deltaTime) override;

    /**
     * @brief Render PacMan sprite or fallback shape
     *
     * Rendering Steps:
     * 1. Check model validity
     * 2. Get grid position → screen position
     * 3. Select sprite (death or normal)
     * 4. Scale sprite to grid cell size (0.8 × cell)
     * 5. Center sprite on position
     * 6. Draw sprite (or fallback rectangle)
     *
     * Sprite Scaling:
     * - Grid cell: camera scale (e.g., 38px)
     * - Desired size: 80% of cell (30.4px)
     * - Scale factor: desiredSize / spriteWidth
     */
    void draw() override;
};

} // namespace representation

#endif