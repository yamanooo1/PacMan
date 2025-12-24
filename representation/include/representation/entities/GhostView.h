#ifndef PACMAN_GHOSTVIEW_H
#define PACMAN_GHOSTVIEW_H

#include "../core/SpriteAtlas.h"
#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

/**
 * @brief GhostView - renders ghosts with color, direction, and fear mode
 *
 * Visual States:
 * 1. Normal: Colored sprite, 2-frame walking animation
 * 2. Fear (solid): Blue sprite, vulnerable
 * 3. Fear (flashing): Blue/white alternating, warning phase
 *
 * Animation System:
 * - Normal: 2 frames per direction (0.15s each)
 * - Fear solid: 2 blue frames (0.15s each)
 * - Fear flashing: 4 frames (blue0, blue1, white0, white1)
 *   Flashing rate: 0.2s per color switch
 *
 * Ghost Personalities (color coding):
 * - RED: Chaser (direct pursuit)
 * - PINK: Ambusher (predictive)
 * - CYAN: Ambusher (cut-off)
 * - ORANGE: Chaser (conditional)
 *
 * Fear Mode Phases:
 * ```
 * Time:    7.0s → 2.0s → 0.0s
 * Visual:  Solid Blue  | Flashing | Normal
 * Danger:  Safe        | Warning  | Deadly
 * ```
 *
 * Sprite Selection Logic:
 * ```cpp
 * if (shouldShowFearMode()) {
 *   if (isFearModeEnding() && showWhite) {
 *     sprite = fearSprite[2 or 3];  // White
 *   } else {
 *     sprite = fearSprite[0 or 1];  // Blue
 *   }
 * } else {
 *   sprite = ghostSprite[color][dir][frame];
 * }
 * ```
 *
 * Flashing Implementation:
 * - flashTimer accumulates deltaTime
 * - When >= 0.2s: toggle showWhite flag
 * - showWhite selects white vs blue sprites
 * - Only active when isFearModeEnding() == true
 *
 * @see Ghost (logic) for fear mode state machine
 * @see SpriteAtlas for sprite frame definitions
 */
class GhostView : public EntityView {
private:
    sf::CircleShape shape;         // Fallback colored circle
    SpriteGhostType spriteType;    // RED, PINK, CYAN, ORANGE

    // Animation state
    float animationTimer;          // Frame timer
    int currentFrame;              // 0 or 1 (2-frame animation)
    float frameDuration;           // 0.15s per frame

    // Fear mode flashing
    float flashTimer;              // Flash cycle timer
    bool showWhite;                // true = white, false = blue

public:
    /**
     * @brief Initialize GhostView with red fallback color
     *
     * Default sprite type: RED (configured by ConcreteFactory)
     *
     * @param model Ghost model to render
     * @param win SFML window
     * @param cam Camera for transformation
     * @param atlas Sprite atlas
     */
    GhostView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
              std::shared_ptr<SpriteAtlas> atlas, int z = 100);  // Default z=100 (foreground)

    /**
     * @brief Handle game events (currently no-op)
     *
     * Ghost state changes handled via direct model queries.
     */
    void update(logic::GameEvent) override;

    /**
     * @brief Update animation frames and fear flashing
     *
     * Frame Animation:
     * - Alternate between frame 0 and 1 every 0.15s
     * - Same timing for normal and fear mode
     *
     * Fear Flashing:
     * - Active when shouldShowFearMode() && isFearModeEnding()
     * - Toggle blue/white every 0.2s
     * - Reset when not in ending phase
     *
     * @param deltaTime Time since last frame
     */
    void updateAnimation(float deltaTime) override;

    /**
     * @brief Render ghost sprite with color/fear state
     *
     * Rendering Steps:
     * 1. Check model validity
     * 2. Get center position (ghosts rendered from center)
     * 3. Query fear mode state from model
     * 4. Select sprite (normal or fear)
     * 5. Scale to 80% of grid cell
     * 6. Draw sprite or fallback circle
     *
     * Fear Mode Query:
     * - shouldShowFearMode(): Currently in fear mode
     * - isFearModeEnding(): Last 2 seconds (flashing)
     * - Uses Ghost* cast to access fear methods
     */
    void draw() override;

    /**
     * @brief Set fallback circle color
     *
     * Used when sprites fail to load.
     * Called by ConcreteFactory with ghost-specific color.
     *
     * @param color SFML color (Red, Magenta, Cyan, Orange)
     */
    void setColor(const sf::Color& color) { shape.setFillColor(color); }

    /**
     * @brief Set sprite type for atlas lookup
     *
     * Called by ConcreteFactory based on ghost color.
     *
     * @param type Ghost color type for sprite selection
     */
    void setSpriteType(SpriteGhostType type) { spriteType = type; }
};

} // namespace representation

#endif