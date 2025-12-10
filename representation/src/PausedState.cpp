#include "representation/PausedState.h"
#include "representation/StateManager.h"
#include "representation/MenuState.h"
#include "representation/SoundManager.h"  // ✅ NEW: Include SoundManager
#include <iostream>

PausedState::PausedState()
    : fontLoaded(false)
    , resumeRequested(false)
    , menuRequested(false)
    , windowWidth(800.0f)
    , windowHeight(860.0f)
{
  if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
    fontLoaded = true;
    std::cout << "[PausedState] Font loaded successfully" << std::endl;
  } else {
    std::cerr << "[PausedState] Failed to load font!" << std::endl;
  }

  setupTexts();
}

void PausedState::onWindowResize(float width, float height) {
  windowWidth = width;
  windowHeight = height;

  std::cout << "[PausedState] Window resized to " << width << "x" << height
            << " - recalculating positions" << std::endl;

  setupTexts();
}

void PausedState::setupTexts() {
  if (!fontLoaded) return;

  // Paused title
  pausedText.setFont(font);
  pausedText.setString("PAUSED");
  pausedText.setCharacterSize(50);
  pausedText.setFillColor(sf::Color::Yellow);

  // Proportional: centered horizontally, 29% from top
  sf::FloatRect pausedBounds = pausedText.getLocalBounds();
  pausedText.setOrigin(pausedBounds.width / 2.f, pausedBounds.height / 2.f);
  pausedText.setPosition(windowWidth * 0.5f, windowHeight * 0.29f);

  // Resume instruction
  resumeText.setFont(font);
  resumeText.setString("Press SPACE to Resume");
  resumeText.setCharacterSize(18);
  resumeText.setFillColor(sf::Color::White);

  // Proportional: centered horizontally, 46.5% from top
  sf::FloatRect resumeBounds = resumeText.getLocalBounds();
  resumeText.setOrigin(resumeBounds.width / 2.f, resumeBounds.height / 2.f);
  resumeText.setPosition(windowWidth * 0.5f, windowHeight * 0.465f);

  // Return to menu instruction
  menuText.setFont(font);
  menuText.setString("Press M for Menu");
  menuText.setCharacterSize(18);
  menuText.setFillColor(sf::Color::Cyan);

  // Proportional: centered horizontally, 55.8% from top
  sf::FloatRect menuBounds = menuText.getLocalBounds();
  menuText.setOrigin(menuBounds.width / 2.f, menuBounds.height / 2.f);
  menuText.setPosition(windowWidth * 0.5f, windowHeight * 0.558f);
}

void PausedState::onEnter() {
  std::cout << "[PausedState] Game paused" << std::endl;

  // ✅ NEW: Play pause music
  SoundManager& soundManager = SoundManager::getInstance();
  soundManager.stopMusic();  // Stop game music
  soundManager.playPauseMusic(true);  // Start pause music (looping)
}

void PausedState::onExit() {
  std::cout << "[PausedState] Resuming game" << std::endl;

  // ✅ NEW: Stop pause music
  SoundManager& soundManager = SoundManager::getInstance();
  soundManager.stopMusic();
}

void PausedState::handleEvents(sf::RenderWindow& window) {
  static bool spaceWasPressed = false;
  static bool mWasPressed = false;

  bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
  bool mIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);

  // Resume with SPACE
  if (spaceIsPressed && !spaceWasPressed) {
    if (stateManager) {
      std::cout << "[PausedState] Resuming..." << std::endl;
      stateManager->popState();
    }
  }

  // Return to menu with M (deferred)
  if (mIsPressed && !mWasPressed) {
    if (stateManager) {
      std::cout << "[PausedState] Returning to menu..." << std::endl;
      stateManager->clearAndPushState(std::make_unique<MenuState>());
    }
  }

  spaceWasPressed = spaceIsPressed;
  mWasPressed = mIsPressed;
}

void PausedState::update(float deltaTime) {
  // Nothing to update in pause menu
}

void PausedState::render(sf::RenderWindow& window) {
  if (!fontLoaded) return;

  // Draw semi-transparent overlay
  sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
  overlay.setFillColor(sf::Color(0, 0, 0, 180));  // Semi-transparent black
  window.draw(overlay);

  // Draw pause menu text
  window.draw(pausedText);
  window.draw(resumeText);
  window.draw(menuText);
}