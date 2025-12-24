#ifndef PACMAN_MENUSTATE_H
#define PACMAN_MENUSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>

namespace representation {

/**
 * @brief MenuState - main menu with buttons and leaderboard
 *
 * Components:
 * 1. Title: "PAC-MAN" (yellow, large)
 * 2. Play Button: Start game → LevelState(1)
 * 3. Exit Button: Close application
 * 4. How to Play: Controls and scoring info
 * 5. Leaderboard: Top 5 high scores
 *
 * UI Layout (800×860):
 * ```
 * ┌────────────────────────┐
 * │      PAC-MAN           │ Title (6%)
 * ├────────────────────────┤
 * │   HOW TO PLAY          │ Section (17%)
 * │   Controls: WASD...    │
 * │   Points: Coin 10-30..  │
 * ├────────────────────────┤
 * │  ┏━━━━━━━━━━━━━━━┓     │
 * │  ┃ TOP 5 SCORES  ┃     │ Leaderboard (42-74%)
 * │  ┃ 1. PLAYER 1000┃     │
 * │  ┃ 2. PLAYER  800┃     │
 * │  ┗━━━━━━━━━━━━━━━┛     │
 * ├────────────────────────┤
 * │     [  PLAY  ]         │ Play button (79%)
 * │     [  EXIT  ]         │ Exit button (88%)
 * └────────────────────────┘
 * ```
 *
 * Button Interaction:
 * - Hover detection: Mouse position vs button bounds
 * - Hover effect: Lighter color (90,140,90 vs 60,100,60)
 * - Click detection: Mouse button released while hovering
 * - Static tracking: Prevents double-click
 *
 * Leaderboard Display:
 * - Loads from Score system (highscores.txt)
 * - Shows top 5 entries
 * - Format: "N. NAME" (left) + "SCORE" (right aligned)
 * - Empty slots: "N. ------" / "---" (gray)
 *
 * Window Resize:
 * - All UI elements repositioned as % of window size
 * - Buttons maintain aspect ratio
 * - Text re-centered
 *
 * Font Loading:
 * - Uses emulogic.ttf (retro pixel font)
 * - fontLoaded flag prevents crashes if load fails
 * - All text rendering checks fontLoaded first
 *
 * State Lifecycle:
 * - onEnter(): Reload leaderboard, stop all music
 * - handleEvents(): Check button clicks
 * - render(): Draw all UI elements
 *
 * @see LevelState for game transition
 * @see Score for high score management
 */
class MenuState : public State {
private:
    sf::Font font;
    sf::Text titleText;

    // Play button
    sf::RectangleShape playButton;
    sf::Text playButtonText;
    bool isPlayButtonHovered;

    // Exit button
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;
    bool isExitButtonHovered;

    // Instructions
    sf::Text howToPlayTitle;
    sf::Text howToPlayControls;
    sf::Text howToPlayPoints;

    // Leaderboard
    sf::RectangleShape leaderboardBox;
    sf::Text leaderboardTitle;
    std::vector<sf::Text> scoreNameTexts;  // Left side: "1. PLAYER"
    std::vector<sf::Text> scoreValueTexts; // Right side: "1000"

    bool fontLoaded;

    float windowWidth;
    float windowHeight;

public:
    MenuState();
    ~MenuState() override = default;

    void onEnter() override;
    void onWindowResize(float width, float height) override;

    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    /**
     * @brief Setup title text (called on init and resize)
     */
    void setupTexts();

    /**
     * @brief Create play button with hover effect
     */
    void setupPlayButton();

    /**
     * @brief Create exit button with hover effect
     */
    void setupExitButton();

    /**
     * @brief Setup controls and scoring instructions
     */
    void setupHowToPlay();

    /**
     * @brief Load and display top 5 high scores
     *
     * Reads from Score system, formats with alignment.
     */
    void setupLeaderboard();

    /**
     * @brief Check if mouse is over button bounds
     *
     * @param button Button shape to test
     * @param window Window for mouse position
     * @return true if mouse inside button
     */
    bool isMouseOverButton(const sf::RectangleShape& button, sf::RenderWindow& window);
};

} // namespace representation

#endif