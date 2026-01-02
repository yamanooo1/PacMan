#include "../../include/representation/states/PausedState.h"
#include "../../include/representation/states/MenuState.h"
#include "../../include/representation/states/StateManager.h"
#include "../../include/representation/systems/SoundManager.h"

namespace representation {

/**
 * @brief Initialize PausedState with default window size
 */
PausedState::PausedState()
    : fontLoaded(false), resumeRequested(false), menuRequested(false), windowWidth(800.0f), windowHeight(860.0f) {
    if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
        fontLoaded = true;
    }

    setupTexts();
}

/**
 * @brief Rebuild UI for new window dimensions
 */
void PausedState::onWindowResize(float width, float height) {
    windowWidth = width;
    windowHeight = height;

    setupTexts();
}

/**
 * @brief Setup centered pause text elements
 *
 * Text hierarchy:
 * 1. "PAUSED" - Large yellow (50pt) at 29%
 * 2. "Press SPACE to Resume" - Medium white (18pt) at 46.5%
 * 3. "Press M for Menu" - Medium cyan (18pt) at 55.8%
 *
 * All texts centered horizontally.
 */
void PausedState::setupTexts() {
    if (!fontLoaded)
        return;

    pausedText.setFont(font);
    pausedText.setString("PAUSED");
    pausedText.setCharacterSize(50);
    pausedText.setFillColor(sf::Color::Yellow);

    sf::FloatRect pausedBounds = pausedText.getLocalBounds();
    pausedText.setOrigin(pausedBounds.width / 2.f, pausedBounds.height / 2.f);
    pausedText.setPosition(windowWidth * 0.5f, windowHeight * 0.29f);

    resumeText.setFont(font);
    resumeText.setString("Press SPACE to Resume");
    resumeText.setCharacterSize(18);
    resumeText.setFillColor(sf::Color::White);

    sf::FloatRect resumeBounds = resumeText.getLocalBounds();
    resumeText.setOrigin(resumeBounds.width / 2.f, resumeBounds.height / 2.f);
    resumeText.setPosition(windowWidth * 0.5f, windowHeight * 0.465f);

    menuText.setFont(font);
    menuText.setString("Press M for Menu");
    menuText.setCharacterSize(18);
    menuText.setFillColor(sf::Color::Cyan);

    sf::FloatRect menuBounds = menuText.getLocalBounds();
    menuText.setOrigin(menuBounds.width / 2.f, menuBounds.height / 2.f);
    menuText.setPosition(windowWidth * 0.5f, windowHeight * 0.558f);
}

/**
 * @brief Start pause music, stop game music
 */
    void PausedState::onEnter() {
        SoundManager& soundManager = SoundManager::getInstance();
        soundManager.stopMusic();
        soundManager.stopAllSounds();  // Stop ook sound effects
        soundManager.playPauseMusic(true);
    }

/**
 * @brief Stop pause music (LevelState will resume its music)
 */
void PausedState::onExit() {
    SoundManager& soundManager = SoundManager::getInstance();
    soundManager.stopMusic();
}

/**
 * @brief Handle resume (SPACE) and menu (M) inputs
 *
 * Static tracking:
 * - spaceWasPressed: Prevent holding SPACE to repeatedly resume
 * - mWasPressed: Prevent holding M to spam menu
 *
 * Actions:
 * - SPACE: popState() → return to LevelState
 * - M: clearAndPushState(MenuState) → abandon game, go to menu
 */
void PausedState::handleEvents(sf::RenderWindow& window) {
    static bool spaceWasPressed = false;
    static bool mWasPressed = false;

    bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    bool mIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);

    // Resume on SPACE release
    if (spaceIsPressed && !spaceWasPressed) {
        if (stateManager) {
            stateManager->popState();
        }
    }

    // Menu on M release
    if (mIsPressed && !mWasPressed) {
        if (stateManager) {
            stateManager->clearAndPushState(std::make_unique<MenuState>());
        }
    }

    spaceWasPressed = spaceIsPressed;
    mWasPressed = mIsPressed;
}

void PausedState::update(float deltaTime) {}

/**
 * @brief Render dark overlay with pause text
 *
 * Overlay: Black rectangle with alpha 180 (semi-transparent)
 * Allows game beneath to be visible but darkened.
 */
void PausedState::render(sf::RenderWindow& window) {
    if (!fontLoaded)
        return;

    // Dark overlay (70% opacity)
    sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    window.draw(pausedText);
    window.draw(resumeText);
    window.draw(menuText);
}

} // namespace representation