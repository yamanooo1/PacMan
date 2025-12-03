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
  // ✅ NEW: Save high score if game is over
  if (isGameOver && score > 0) {
    Score tempScore;
    tempScore.loadHighScores();

    if (tempScore.isHighScore(score)) {
      std::cout << "[VictoryState] NEW HIGH SCORE: " << score << "!" << std::endl;
      tempScore.addHighScore(score);
    }
  }

  if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
    fontLoaded = true;
    std::cout << "[VictoryState] Font loaded successfully" << std::endl;
  } else {
    std::cerr << "[VictoryState] Failed to load font!" << std::endl;
  }
  
  setupTexts();
}

void VictoryState::setupTexts() {
  if (!fontLoaded) return;
  
  // Title (different for game over vs level cleared)
  titleText.setFont(font);
  if (isGameOver) {
    titleText.setString("GAME OVER");
    titleText.setFillColor(sf::Color::Red);
  } else {
    titleText.setString("LEVEL CLEARED!");
    titleText.setFillColor(sf::Color::Yellow);
  }
  titleText.setCharacterSize(40);
  titleText.setPosition(180.f, 250.f);
  
  // Score display
  scoreText.setFont(font);
  scoreText.setString("Final Score: " + std::to_string(finalScore));
  scoreText.setCharacterSize(20);
  scoreText.setFillColor(sf::Color::White);
  scoreText.setPosition(220.f, 350.f);
  
  // Continue instruction (only if not game over)
  continueText.setFont(font);
  if (isGameOver) {
    continueText.setString("");
  } else {
    continueText.setString("Press SPACE for Next Level");
    continueText.setCharacterSize(16);
    continueText.setFillColor(sf::Color::Green);
    continueText.setPosition(160.f, 450.f);
  }
  
  // Return to menu
  menuText.setFont(font);
  menuText.setString("Press M for Menu");
  menuText.setCharacterSize(16);
  menuText.setFillColor(sf::Color::Cyan);
  menuText.setPosition(240.f, 520.f);
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

      // Clear all states and push MenuState
      stateManager->clearStates();
      stateManager->pushState(std::make_unique<MenuState>());
    }
  }

  spaceWasPressed = spaceIsPressed;
  mWasPressed = mIsPressed;
}

void VictoryState::update(float deltaTime) {
  // Nothing to update
}

void VictoryState::render(sf::RenderWindow& window) {
  if (!fontLoaded) return;
  
  // Draw semi-transparent overlay
  sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
  overlay.setFillColor(sf::Color(0, 0, 0, 200));  // Dark overlay
  window.draw(overlay);
  
  // Draw victory/game over screen
  window.draw(titleText);
  window.draw(scoreText);
  window.draw(continueText);
  window.draw(menuText);
}