#include "representation/MenuState.h"
#include "representation/StateManager.h"
#include "representation/LevelState.h"
#include "logic/Score.h"
#include <iostream>

MenuState::MenuState()
    : fontLoaded(false)
{
  if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
    fontLoaded = true;
    std::cout << "[MenuState] Font loaded successfully" << std::endl;
  } else {
    std::cerr << "[MenuState] Failed to load font!" << std::endl;
  }

  setupTexts();
}

void MenuState::setupTexts() {
  if (!fontLoaded) return;

  // Title
  titleText.setFont(font);
  titleText.setString("PAC-MAN");
  titleText.setCharacterSize(50);
  titleText.setFillColor(sf::Color::Yellow);

  // Center title horizontally
  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setPosition((800.f - titleBounds.width) / 2.f, 80.f);

  // Play button/instruction
  playText.setFont(font);
  playText.setString("Press SPACE to Play");
  playText.setCharacterSize(18);
  playText.setFillColor(sf::Color::White);

  sf::FloatRect playBounds = playText.getLocalBounds();
  playText.setPosition((800.f - playBounds.width) / 2.f, 250.f);

  // Instructions
  instructionsText.setFont(font);
  instructionsText.setString("Arrow Keys / WASD to move\nESC to pause");
  instructionsText.setCharacterSize(12);
  instructionsText.setFillColor(sf::Color::Cyan);

  sf::FloatRect instrBounds = instructionsText.getLocalBounds();
  instructionsText.setPosition((800.f - instrBounds.width) / 2.f, 320.f);

  // Leaderboard title
  leaderboardTitle.setFont(font);
  leaderboardTitle.setString("TOP SCORES");
  leaderboardTitle.setCharacterSize(20);
  leaderboardTitle.setFillColor(sf::Color::Yellow);

  sf::FloatRect leaderBounds = leaderboardTitle.getLocalBounds();
  leaderboardTitle.setPosition((800.f - leaderBounds.width) / 2.f, 450.f);
}

void MenuState::onEnter() {
  std::cout << "[MenuState] Entered menu state" << std::endl;

  // ✅ Load high scores ONCE when entering menu
  Score tempScore;
  tempScore.loadHighScores();
  highScores = tempScore.getHighScores();

  // ✅ Create score texts
  scoreTexts.clear();
  float yPosition = 500.f;

  for (size_t i = 0; i < highScores.size(); ++i) {
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString(std::to_string(i + 1) + ".  " + std::to_string(highScores[i]));
    scoreText.setCharacterSize(16);
    scoreText.setFillColor(sf::Color::White);

    sf::FloatRect bounds = scoreText.getLocalBounds();
    scoreText.setPosition((800.f - bounds.width) / 2.f, yPosition);

    scoreTexts.push_back(scoreText);
    yPosition += 35.f;
  }

  // Add empty slots
  for (size_t i = highScores.size(); i < 5; ++i) {
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString(std::to_string(i + 1) + ".  ---");
    scoreText.setCharacterSize(16);
    scoreText.setFillColor(sf::Color(100, 100, 100));

    sf::FloatRect bounds = scoreText.getLocalBounds();
    scoreText.setPosition((800.f - bounds.width) / 2.f, yPosition);

    scoreTexts.push_back(scoreText);
    yPosition += 35.f;
  }
}

void MenuState::onExit() {
  std::cout << "[MenuState] Exited menu state" << std::endl;
}

void MenuState::handleEvents(sf::RenderWindow& window) {
  static bool spaceWasPressed = false;
  static bool escWasPressed = false;

  bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
  bool escIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

  // Check for Space key to start game (detect press)
  if (spaceIsPressed && !spaceWasPressed) {
    std::cout << "[MenuState] Starting game..." << std::endl;

    // Transition to LevelState
    if (stateManager) {
      stateManager->pushState(std::make_unique<LevelState>());
    }
  }

  // ESC to quit from menu (detect press)
  if (escIsPressed && !escWasPressed) {
    std::cout << "[MenuState] Quitting game..." << std::endl;
    window.close();
  }

  spaceWasPressed = spaceIsPressed;
  escWasPressed = escIsPressed;
}

void MenuState::update(float deltaTime) {
  // Menu doesn't need to update anything for now
}

void MenuState::render(sf::RenderWindow& window) {
  if (!fontLoaded) return;

  // Draw all text elements
  window.draw(titleText);
  window.draw(playText);
  window.draw(instructionsText);
  window.draw(leaderboardTitle);

  // ✅ Draw pre-loaded score texts (no file I/O every frame!)
  for (const auto& scoreText : scoreTexts) {
    window.draw(scoreText);
  }
}