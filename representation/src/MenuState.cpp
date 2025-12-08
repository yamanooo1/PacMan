#include "representation/MenuState.h"
#include "representation/StateManager.h"
#include "representation/LevelState.h"
#include "logic/Score.h"
#include <iostream>

MenuState::MenuState()
    : fontLoaded(false)
    , isPlayButtonHovered(false)
{
  if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
    fontLoaded = true;
    std::cout << "[MenuState] Font loaded successfully" << std::endl;
  } else {
    std::cerr << "[MenuState] Failed to load font!" << std::endl;
  }

  setupTexts();
  setupPlayButton();
  setupHowToPlay();
}

void MenuState::setupTexts() {
  if (!fontLoaded) return;

  // Title - larger and more prominent
  titleText.setFont(font);
  titleText.setString("PAC-MAN");
  titleText.setCharacterSize(60);
  titleText.setFillColor(sf::Color::Yellow);

  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setPosition((800.f - titleBounds.width) / 2.f, 50.f);
}

// ✅ NEW: Setup clickable play button
void MenuState::setupPlayButton() {
  if (!fontLoaded) return;

  // Button shape (grey, will lighten on hover)
  playButton.setSize(sf::Vector2f(300.f, 60.f));
  playButton.setFillColor(sf::Color(80, 80, 80));  // Dark grey
  playButton.setOutlineColor(sf::Color::White);
  playButton.setOutlineThickness(3.f);

  // Center button horizontally, near bottom
  playButton.setPosition((800.f - 300.f) / 2.f, 680.f);

  // Button text
  playButtonText.setFont(font);
  playButtonText.setString("PLAY");
  playButtonText.setCharacterSize(28);
  playButtonText.setFillColor(sf::Color::White);

  // Center text in button
  sf::FloatRect textBounds = playButtonText.getLocalBounds();
  playButtonText.setPosition(
    playButton.getPosition().x + (300.f - textBounds.width) / 2.f - textBounds.left,
    playButton.getPosition().y + (60.f - textBounds.height) / 2.f - textBounds.top - 5.f
  );
}

// ✅ NEW: Setup how to play section
void MenuState::setupHowToPlay() {
  if (!fontLoaded) return;

  // Title
  howToPlayTitle.setFont(font);
  howToPlayTitle.setString("HOW TO PLAY");
  howToPlayTitle.setCharacterSize(20);
  howToPlayTitle.setFillColor(sf::Color::Cyan);

  sf::FloatRect titleBounds = howToPlayTitle.getLocalBounds();
  howToPlayTitle.setPosition((800.f - titleBounds.width) / 2.f, 150.f);

  // Controls text
  howToPlayControls.setFont(font);
  howToPlayControls.setString(
    "ARROW KEYS or WASD - Move Pac-Man\n"
    "      ESC - Pause Game\n"
    "      M - Return to Menu\n\n"
    " Eat all dots to clear level!\n"
    "Eat fruit for fear mode power!"
  );
  howToPlayControls.setCharacterSize(12);
  howToPlayControls.setFillColor(sf::Color::White);
  howToPlayControls.setLineSpacing(1.5f);

  sf::FloatRect controlsBounds = howToPlayControls.getLocalBounds();
  howToPlayControls.setPosition((800.f - controlsBounds.width) / 2.f, 190.f);
}

void MenuState::onEnter() {
  std::cout << "[MenuState] Entered menu state" << std::endl;

  // Load and setup leaderboard
  setupLeaderboard();
}

// ✅ NEW: Setup leaderboard with yellow box
void MenuState::setupLeaderboard() {
  if (!fontLoaded) return;

  // Load high scores
  Score tempScore;
  tempScore.loadHighScores();
  auto highScores = tempScore.getHighScores();

  // Yellow box for leaderboard
  leaderboardBox.setSize(sf::Vector2f(600.f, 280.f));
  leaderboardBox.setFillColor(sf::Color(255, 255, 0, 40));  // Semi-transparent yellow
  leaderboardBox.setOutlineColor(sf::Color::Yellow);
  leaderboardBox.setOutlineThickness(4.f);
  leaderboardBox.setPosition((800.f - 600.f) / 2.f, 360.f);

  // Title
  leaderboardTitle.setFont(font);
  leaderboardTitle.setString("TOP 5 SCORES");
  leaderboardTitle.setCharacterSize(24);
  leaderboardTitle.setFillColor(sf::Color::Yellow);

  sf::FloatRect titleBounds = leaderboardTitle.getLocalBounds();
  leaderboardTitle.setPosition((800.f - titleBounds.width) / 2.f, 380.f);

  // Create score entries
  scoreNameTexts.clear();
  scoreValueTexts.clear();

  float yPosition = 430.f;
  float leftX = 150.f;    // Position for player names (left side)
  float rightX = 650.f;   // Position for scores (right side)

  for (size_t i = 0; i < 5; ++i) {
    // Player name (left side)
    sf::Text nameText;
    nameText.setFont(font);
    nameText.setCharacterSize(16);

    if (i < highScores.size()) {
      // Format: "1. PLAYERNAME"
      nameText.setString(std::to_string(i + 1) + ". " + highScores[i].playerName);
      nameText.setFillColor(sf::Color::White);
    } else {
      // Empty slot
      nameText.setString(std::to_string(i + 1) + ". ------");
      nameText.setFillColor(sf::Color(100, 100, 100));
    }

    nameText.setPosition(leftX, yPosition);
    scoreNameTexts.push_back(nameText);

    // Score (right side)
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(16);

    if (i < highScores.size()) {
      scoreText.setString(std::to_string(highScores[i].score));
      scoreText.setFillColor(sf::Color::White);
    } else {
      scoreText.setString("---");
      scoreText.setFillColor(sf::Color(100, 100, 100));
    }

    // Right-align the score
    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setPosition(rightX - scoreBounds.width, yPosition);
    scoreValueTexts.push_back(scoreText);

    yPosition += 45.f;
  }
}

void MenuState::onExit() {
  std::cout << "[MenuState] Exited menu state" << std::endl;
}

void MenuState::handleEvents(sf::RenderWindow& window) {
  static bool mouseWasPressed = false;
  bool mouseIsPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

  // ✅ NEW: Check for play button click
  if (mouseIsPressed && !mouseWasPressed) {
    if (isMouseOverButton(playButton, window)) {
      std::cout << "[MenuState] Play button clicked - starting game..." << std::endl;

      if (stateManager) {
        stateManager->pushState(std::make_unique<LevelState>(1));
      }
    }
  }

  mouseWasPressed = mouseIsPressed;

  // Keep ESC to quit as backup
  static bool escWasPressed = false;
  bool escIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

  if (escIsPressed && !escWasPressed) {
    std::cout << "[MenuState] Quitting game..." << std::endl;
    window.close();
  }

  escWasPressed = escIsPressed;
}

void MenuState::update(float deltaTime) {
  // Nothing to update - hover state is checked in render()
}

bool MenuState::isMouseOverButton(const sf::RectangleShape& button, sf::RenderWindow& window) {
  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  sf::Vector2f mousePosFloat(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

  return button.getGlobalBounds().contains(mousePosFloat);
}

void MenuState::render(sf::RenderWindow& window) {
  if (!fontLoaded) return;

  // Update hover state (needs window)
  bool wasHovered = isPlayButtonHovered;
  isPlayButtonHovered = isMouseOverButton(playButton, window);

  if (isPlayButtonHovered) {
    playButton.setFillColor(sf::Color(140, 140, 140));
  } else {
    playButton.setFillColor(sf::Color(80, 80, 80));
  }

  // Draw title
  window.draw(titleText);

  // Draw how to play section
  window.draw(howToPlayTitle);
  window.draw(howToPlayControls);

  // ✅ Draw leaderboard yellow box
  window.draw(leaderboardBox);
  window.draw(leaderboardTitle);

  // Draw score entries (names on left, scores on right)
  for (const auto& nameText : scoreNameTexts) {
    window.draw(nameText);
  }
  for (const auto& scoreText : scoreValueTexts) {
    window.draw(scoreText);
  }

  // ✅ Draw play button (at bottom)
  window.draw(playButton);
  window.draw(playButtonText);
}