#include "representation/PausedState.h"
#include "representation/StateManager.h"
#include "representation/MenuState.h"
#include <iostream>

PausedState::PausedState()
    : fontLoaded(false)
    , resumeRequested(false)
    , menuRequested(false)
{
  if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
    fontLoaded = true;
    std::cout << "[PausedState] Font loaded successfully" << std::endl;
  } else {
    std::cerr << "[PausedState] Failed to load font!" << std::endl;
  }

  setupTexts();
}

void PausedState::setupTexts() {
  if (!fontLoaded) return;

  // Paused title
  pausedText.setFont(font);
  pausedText.setString("PAUSED");
  pausedText.setCharacterSize(50);
  pausedText.setFillColor(sf::Color::Yellow);
  pausedText.setPosition(280.f, 250.f);

  // Resume instruction
  resumeText.setFont(font);
  resumeText.setString("Press SPACE to Resume");
  resumeText.setCharacterSize(18);
  resumeText.setFillColor(sf::Color::White);
  resumeText.setPosition(200.f, 400.f);

  // Return to menu instruction
  menuText.setFont(font);
  menuText.setString("Press M for Menu");
  menuText.setCharacterSize(18);
  menuText.setFillColor(sf::Color::Cyan);
  menuText.setPosition(240.f, 480.f);
}

void PausedState::onEnter() {
  std::cout << "[PausedState] Game paused" << std::endl;
}

void PausedState::onExit() {
  std::cout << "[PausedState] Resuming game" << std::endl;
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

  // ✅ FIXED: Return to menu with M (deferred)
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
  sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
  overlay.setFillColor(sf::Color(0, 0, 0, 180));  // Semi-transparent black
  window.draw(overlay);

  // Draw pause menu text
  window.draw(pausedText);
  window.draw(resumeText);
  window.draw(menuText);
}