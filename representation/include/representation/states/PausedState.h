#ifndef PACMAN_PAUSEDSTATE_H
#define PACMAN_PAUSEDSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>

namespace representation {

/**
 * @brief PausedState - transparent pause overlay
 *
 * DESIGN: Transparent state (renders over LevelState)
 *
 * Display:
 * ```
 * ┌─────────────────────┐
 * │                     │
 * │      PAUSED         │ ← 29% from top
 * │                     │
 * │  Press SPACE to     │ ← 46.5%
 * │      Resume         │
 * │                     │
 * │  Press M for Menu   │ ← 55.8%
 * │                     │
 * └─────────────────────┘
 * Dark overlay (alpha 180)
 * ```
 *
 * Transparency:
 * - isTransparent() returns true
 * - StateManager renders LevelState first, then PausedState
 * - Game visible behind dark overlay
 *
 * Input Handling:
 * - SPACE: Resume game (pop this state)
 * - M: Return to menu (clearAndPushState MenuState)
 * - Static key tracking prevents repeat triggers
 *
 * Music Control:
 * - onEnter(): Stop game music, start pause music
 * - onExit(): Stop pause music (LevelState resumes its music)
 *
 * @see State::isTransparent() for rendering behavior
 * @see LevelState for resumed state
 */
class PausedState : public State {
private:
    sf::Font font;
    sf::Text pausedText;
    sf::Text resumeText;
    sf::Text menuText;

    bool fontLoaded;
    bool resumeRequested;
    bool menuRequested;

    float windowWidth;
    float windowHeight;

public:
    PausedState();
    ~PausedState() override = default;

    void onEnter() override;
    void onExit() override;
    void onWindowResize(float width, float height) override;

    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    /**
     * @brief Mark state as transparent
     *
     * Allows game to be visible behind pause overlay.
     *
     * @return true (always transparent)
     */
    bool isTransparent() const override { return true; }

private:
    /**
     * @brief Setup all text elements with proper positioning
     */
    void setupTexts();
};

} // namespace representation

#endif