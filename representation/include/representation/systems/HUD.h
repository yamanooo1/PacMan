#ifndef HUD_H
#define HUD_H

#include <SFML/Graphics.hpp>
#include <string>

namespace logic {
class World;
class Score;
class Lives;
} // namespace logic

namespace representation {

/**
 * @brief HUD - Heads-Up Display for game UI
 *
 * Purpose:
 * - Display score, lives, and level number
 * - Show special text overlays (READY!, LEVEL CLEARED!)
 * - Fixed 60px height bar at bottom of window
 *
 * Layout (800px width):
 * ```
 * ┌──────────────────────────────────────┐
 * │ SCORE: 1234  │  LEVEL 2  │ LIVES: 3 │
 * │   (left)     │  (center) │  (right) │
 * └──────────────────────────────────────┘
 *      20px          center        20px
 * ```
 *
 * Text Overlays:
 * 1. **READY!** (yellow, 24pt)
 *    - Displayed during World::readyStateActive
 *    - Position: Grid (10.5, 10.4) → screen coords
 *    - Duration: 4.7 seconds before gameplay
 *
 * 2. **LEVEL CLEARED!** (green, 30pt)
 *    - Displayed during World::levelClearedDisplayActive
 *    - Position: Grid (10.5, 10.5) → screen coords
 *    - Duration: 3 seconds before transition
 *
 * Grid-to-Screen Conversion:
 * - Grid: Logic coordinate system (0-21)
 * - Screen: Pixel coordinates
 * - Formula: screenX = gridX × (gameWidth / 21.0)
 * - Note: HUD height subtracted from window height for game area
 *
 * Font Loading:
 * - Uses emulogic.ttf (retro pixel font)
 * - fontLoaded flag prevents rendering if load fails
 * - Graceful degradation: no HUD if font missing
 *
 * @see logic::World for game state queries
 * @see logic::Score for score data
 * @see logic::Lives for lives data
 */
class HUD {
public:
    /**
     * @brief Initialize HUD with window and height
     *
     * @param window SFML window for rendering
     * @param hudHeight Fixed HUD bar height (60px)
     */
    HUD(sf::RenderWindow& window, int hudHeight);

    /**
     * @brief Load pixel font from file
     *
     * Called by LevelState after HUD creation.
     *
     * @param fontPath Path to .ttf file
     */
    void loadFont(const std::string& fontPath);

    /**
     * @brief Draw main HUD bar (score, lives, level)
     *
     * Draws:
     * - Black background bar (full width × hudHeight)
     * - Score text (left aligned, 20px from left)
     * - Level text (center aligned)
     * - Lives text (right aligned, 20px from right)
     *
     * @param world World for state queries (unused here)
     * @param score Score system for current score
     * @param lives Lives system for remaining lives
     * @param currentLevel Level number to display
     */
    void draw(logic::World* world, logic::Score* score, logic::Lives* lives, int currentLevel);

    /**
     * @brief Draw "READY!" text overlay
     *
     * Only draws if world->isReadyStateActive().
     * Text centered on grid position (10.5, 10.4).
     *
     * @param world World for ready state check
     */
    void drawReadyText(logic::World* world);

    /**
     * @brief Draw "LEVEL CLEARED!" text overlay
     *
     * Only draws if world->isLevelClearedDisplayActive().
     * Text centered on grid position (10.5, 10.5).
     *
     * @param world World for cleared state check
     */
    void drawLevelClearedText(logic::World* world);

private:
    sf::RenderWindow& window;
    sf::Font font;
    int hudHeight; // 60px fixed
    bool fontLoaded;
};

} // namespace representation

#endif