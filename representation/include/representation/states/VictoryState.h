#ifndef PACMAN_VICTORYSTATE_H
#define PACMAN_VICTORYSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace representation {

/**
 * @brief VictoryState - handles level complete and game over
 *
 * Two Modes:
 * 1. **Level Complete** (nextLevel > 0):
 *    - Title: "LEVEL CLEARED!" (yellow)
 *    - Options: SPACE for next level, M for menu
 *
 * 2. **Game Over** (nextLevel == 0):
 *    - Title: "GAME OVER" (red)
 *    - High score entry if qualified
 *    - Only M for menu (no continue)
 *
 * High Score Entry System:
 * ```
 * Qualification Check:
 *   if (gameOver && score > 0)
 *     if (score > 5th place OR < 5 entries)
 *       → Enable name input
 *
 * Input UI:
 * ┌─────────────────────────┐
 * │  NEW HIGH SCORE!        │
 * │  Enter your name:       │
 * │  ┏━━━━━━━━━━━━━━━━━━┓  │
 * │  ┃ PLAYER___        ┃  │ ← Text input box
 * │  ┗━━━━━━━━━━━━━━━━━━┛  │
 * │  Use keyboard - Press   │
 * │      ENTER when done    │
 * └─────────────────────────┘
 * ```
 *
 * Name Input:
 * - Max length: 10 characters
 * - Allowed: A-Z, 0-9 (uppercase only)
 * - Backspace: Remove last character
 * - Enter: Submit to high scores
 * - Static key tracking per key (prevents repeat)
 *
 * Key Mapping:
 * - Keyboard::A-Z → 'A'-'Z'
 * - Keyboard::Num0-9 → '0'-'9'
 * - Built in setupNameInput() via map initialization
 *
 * State Transitions:
 * - Level complete + SPACE → LevelState(nextLevel, currentScore)
 * - Any + M → MenuState
 * - Game over + Enter (after name) → MenuState
 *
 * @see Score::isHighScore() for qualification
 * @see Score::addHighScore() for submission
 */
class VictoryState : public State {
private:
    sf::Font font;
    sf::Text titleText;    // "LEVEL CLEARED!" or "GAME OVER"
    sf::Text scoreText;    // "Score: 1234"
    sf::Text continueText; // "Press SPACE for Next Level"
    sf::Text menuText;     // "Press M for Menu"

    // High score entry UI
    sf::Text namePromptText; // "NEW HIGH SCORE!\nEnter your name:"
    sf::Text nameInputText;  // User input display
    sf::RectangleShape nameInputBox;
    std::string playerNameInput;
    bool isEnteringName;
    bool nameSubmitted;

    bool fontLoaded;
    bool continueRequested;
    bool menuRequested;

    bool spaceWasPressed;
    bool mWasPressed;

    int nextLevel; // 0 = game over, >0 = next level number
    int finalScore;
    bool isGameOver;
    bool qualifiesForHighScore;

    float windowWidth;
    float windowHeight;

public:
    /**
     * @brief Initialize VictoryState with level/score info
     *
     * @param nextLevelNum Next level (0 for game over)
     * @param score Final score achieved
     */
    VictoryState(int nextLevelNum, int score);
    ~VictoryState() override = default;

    void onEnter() override;
    void onExit() override;
    void onWindowResize(float width, float height) override;

    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    /**
     * @brief Transparent overlay over game
     *
     * @return true (show game beneath)
     */
    bool isTransparent() const override { return true; }

private:
    /**
     * @brief Setup title and score text
     */
    void setupTexts();

    /**
     * @brief Setup name input box and prompt
     */
    void setupNameInput();

    /**
     * @brief Handle single character input (unused - using direct keyboard)
     *
     * @param unicode UTF-32 character code
     */
    void handleTextInput(sf::Uint32 unicode);

    /**
     * @brief Submit name to high score system
     *
     * Saves to highscores.txt and transitions to menu.
     */
    void submitHighScore();
};

} // namespace representation

#endif