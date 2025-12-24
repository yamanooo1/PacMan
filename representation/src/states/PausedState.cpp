#include "../../include/representation/states/PausedState.h"
#include "../../include/representation/states/MenuState.h"
#include "../../include/representation/states/StateManager.h"
#include "../../include/representation/systems/SoundManager.h"

namespace representation {

PausedState::PausedState()
    : fontLoaded(false), resumeRequested(false), menuRequested(false), windowWidth(800.0f), windowHeight(860.0f) {
    if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
        fontLoaded = true;
    }

    setupTexts();
}

void PausedState::onWindowResize(float width, float height) {
    windowWidth = width;
    windowHeight = height;

    setupTexts();
}

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

void PausedState::onEnter() {
    SoundManager& soundManager = SoundManager::getInstance();
    soundManager.stopMusic();
    soundManager.playPauseMusic(true);
}

void PausedState::onExit() {
    SoundManager& soundManager = SoundManager::getInstance();
    soundManager.stopMusic();
}

void PausedState::handleEvents(sf::RenderWindow& window) {
    static bool spaceWasPressed = false;
    static bool mWasPressed = false;

    bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    bool mIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);

    if (spaceIsPressed && !spaceWasPressed) {
        if (stateManager) {
            stateManager->popState();
        }
    }

    if (mIsPressed && !mWasPressed) {
        if (stateManager) {
            stateManager->clearAndPushState(std::make_unique<MenuState>());
        }
    }

    spaceWasPressed = spaceIsPressed;
    mWasPressed = mIsPressed;
}

void PausedState::update(float deltaTime) {}

void PausedState::render(sf::RenderWindow& window) {
    if (!fontLoaded)
        return;

    sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    window.draw(pausedText);
    window.draw(resumeText);
    window.draw(menuText);
}

} // namespace representation