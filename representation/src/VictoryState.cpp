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
    , nextLevel(nextLevelNum)
    , finalScore(score)
    , isGameOver(nextLevelNum == 0)
{
  std::cout << "[VictoryState] Constructor called - NextLevel: " << nextLevelNum
            << " Score: " << score << " IsGameOver: " << isGameOver << std::endl;

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
  titleText.setPosition(150.f, 250.f);

  // Score display
  scoreText.setFont(font);
  scoreText.setString("Score: " + std::to_string(finalScore));
  scoreText.setCharacterSize(20);
  scoreText.setFillColor(sf::Color::White);
  scoreText.setPosition(250.f, 350.f);
  std::cout << "[VictoryState] Score text: Score: " << finalScore << std::endl;

  // Continue instruction (only if not game over)
  continueText.setFont(font);
  if (isGameOver) {
    continueText.setString("");
    std::cout << "[VictoryState] Continue text: (empty - game over)" << std::endl;
  } else {
    continueText.setString("SPACE - Next Level");
    continueText.setCharacterSize(18);
    continueText.setFillColor(sf::Color::Green);
    continueText.setPosition(200.f, 450.f);
    std::cout << "[VictoryState] Continue text: SPACE - Next Level (green)" << std::endl;
  }

  // Return to menu
  menuText.setFont(font);
  menuText.setString("M - Menu");
  menuText.setCharacterSize(18);
  menuText.setFillColor(sf::Color::Cyan);
  menuText.setPosition(300.f, 520.f);
  std::cout << "[VictoryState] Menu text: M - Menu (cyan)" << std::endl;

  std::cout << "[VictoryState] setupTexts() complete" << std::endl;
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
  static bool spaceWasPressed = false;
  static bool mWasPressed = false;

  bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
  bool mIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);

  // Continue to next level with SPACE (only if not game over)
  if (!isGameOver && spaceIsPressed && !spaceWasPressed) {
    if (stateManager) {
      std::cout << "[VictoryState] Starting level " << nextLevel << "..." << std::endl;

      // Pop VictoryState and LevelState, then push new LevelState
      stateManager->popState();  // Remove VictoryState
      stateManager->popState();  // Remove old LevelState
      stateManager->pushState(std::make_unique<LevelState>(nextLevel));
    }
  }

  // Return to menu with M
  if (mIsPressed && !mWasPressed) {
    if (stateManager) {
      std::cout << "[VictoryState] Returning to menu..." << std::endl;

      // ✅ Save score before going to menu (if not already saved)
      if (!isGameOver && finalScore > 0) {
        Score tempScore;
        tempScore.loadHighScores();

        if (tempScore.isHighScore(finalScore)) {
          std::cout << "[VictoryState] Saving high score: " << finalScore << std::endl;
          tempScore.addHighScore(finalScore);
        }
      }

      // Clear all states and push MenuState
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