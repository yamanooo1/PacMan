#include "representation/VictoryState.h"
#include "representation/StateManager.h"
#include "representation/MenuState.h"
#include "representation/LevelState.h"
#include "logic/Score.h"
#include <iostream>

VictoryState::VictoryState(int nextLevelNum, int score)
    : fontLoaded(false)
    , continueRequested(false)
    , menuRequested(false)
    , spaceWasPressed(false)
    , mWasPressed(false)
    , nextLevel(nextLevelNum)
    , finalScore(score)
    , isGameOver(nextLevelNum == 0)
{
  std::cout << "\n🎉 [VICTORYSTATE] CONSTRUCTOR CALLED!" << std::endl;
  std::cout << "    Next Level: " << nextLevel << std::endl;
  std::cout << "    Final Score: " << finalScore << std::endl;

  // ✅ Save high score when returning to menu from victory
  if (score > 0) {
    Score tempScore;
    tempScore.loadHighScores();

    if (tempScore.isHighScore(score)) {
      std::cout << "[VictoryState] NEW HIGH SCORE: " << score << "!" << std::endl;
      tempScore.addHighScore(score);
    }
  }

  if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
    fontLoaded = true;
    std::cout << "[VictoryState] ✅ Font loaded successfully" << std::endl;
  } else {
    std::cerr << "[VictoryState] ❌ FAILED to load font!" << std::endl;
  }

  setupTexts();
  std::cout << "[VictoryState] Constructor complete" << std::endl;
}

void VictoryState::setupTexts() {
  std::cout << "[VictoryState] setupTexts() called - fontLoaded: " << fontLoaded << std::endl;

  if (!fontLoaded) {
    std::cerr << "[VictoryState] Cannot setup texts - font not loaded!" << std::endl;
    return;
  }

  // Title (different for game over vs level cleared)
  titleText.setFont(font);
  if (isGameOver) {
    titleText.setString("GAME OVER");
    titleText.setFillColor(sf::Color::Red);
    std::cout << "[VictoryState] Title: GAME OVER (red)" << std::endl;
  } else {
    titleText.setString("LEVEL CLEARED!");
    titleText.setFillColor(sf::Color::Yellow);
    std::cout << "[VictoryState] Title: LEVEL CLEARED! (yellow)" << std::endl;
  }
  titleText.setCharacterSize(40);

  // ✅ CENTER the title
  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
  titleText.setPosition(400.f, 250.f);  // Center of 800px window

  // Score display
  scoreText.setFont(font);
  scoreText.setString("Score: " + std::to_string(finalScore));
  scoreText.setCharacterSize(24);  // ✅ Bigger text
  scoreText.setFillColor(sf::Color::White);

  // ✅ CENTER the score
  sf::FloatRect scoreBounds = scoreText.getLocalBounds();
  scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
  scoreText.setPosition(400.f, 350.f);
  std::cout << "[VictoryState] Score text: " << finalScore << std::endl;

  // Continue instruction (only if not game over)
  continueText.setFont(font);
  if (isGameOver) {
    continueText.setString("");
    std::cout << "[VictoryState] Continue text: (empty - game over)" << std::endl;
  } else {
    continueText.setString("Press SPACE for Next Level");
    continueText.setCharacterSize(16);
    continueText.setFillColor(sf::Color::Green);

    // ✅ CENTER the continue text
    sf::FloatRect contBounds = continueText.getLocalBounds();
    continueText.setOrigin(contBounds.width / 2.f, contBounds.height / 2.f);
    continueText.setPosition(400.f, 450.f);
    std::cout << "[VictoryState] Continue text: Press SPACE for Next Level" << std::endl;
  }

  // Return to menu
  menuText.setFont(font);
  menuText.setString("Press M for Menu");
  menuText.setCharacterSize(16);
  menuText.setFillColor(sf::Color::Cyan);

  // ✅ CENTER the menu text
  sf::FloatRect menuBounds = menuText.getLocalBounds();
  menuText.setOrigin(menuBounds.width / 2.f, menuBounds.height / 2.f);
  menuText.setPosition(400.f, 520.f);
  std::cout << "[VictoryState] Menu text: Press M for Menu" << std::endl;

  std::cout << "[VictoryState] setupTexts() complete - ALL TEXT CENTERED" << std::endl;
}

void VictoryState::onEnter() {
  if (isGameOver) {
    std::cout << "[VictoryState] Game Over! Final score: " << finalScore << std::endl;
  } else {
    std::cout << "[VictoryState] Level cleared! Moving to level " << nextLevel << std::endl;
  }
}

void VictoryState::onExit() {
  std::cout << "[VictoryState] Exiting victory state" << std::endl;
}

void VictoryState::handleEvents(sf::RenderWindow& window) {

  bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
  bool mIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);

  // Continue to next level with SPACE (only if not game over)
  if (!isGameOver && spaceIsPressed && !spaceWasPressed) {
    if (stateManager) {
      std::cout << "[VictoryState] ✅ Starting level " << nextLevel
                << " with score " << finalScore << "..." << std::endl;
      stateManager->clearAndPushState(std::make_unique<LevelState>(nextLevel, finalScore));  // ✅ PASS SCORE
    }
  }

  // Return to menu with M
  if (mIsPressed && !mWasPressed) {
    if (stateManager) {
      std::cout << "[VictoryState] Returning to menu..." << std::endl;

      // Score already saved in constructor - no need to save again
      stateManager->clearAndPushState(std::make_unique<MenuState>());
    }
  }

  spaceWasPressed = spaceIsPressed;
  mWasPressed = mIsPressed;
}

void VictoryState::update(float deltaTime) {
  // Nothing to update
}

void VictoryState::render(sf::RenderWindow& window) {
  if (!fontLoaded) {
    std::cerr << "[VictoryState] Font not loaded - cannot render!" << std::endl;
    return;
  }

  // Draw semi-transparent overlay
  sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
  overlay.setFillColor(sf::Color(0, 0, 0, 200));  // Dark overlay
  window.draw(overlay);

  // Draw victory/game over screen
  window.draw(titleText);
  window.draw(scoreText);

  // Only draw continue text if not empty (not game over)
  if (!isGameOver) {
    window.draw(continueText);
  }

  window.draw(menuText);
}