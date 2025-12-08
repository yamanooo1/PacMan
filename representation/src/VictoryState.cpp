#include "representation/VictoryState.h"
#include "representation/StateManager.h"
#include "representation/MenuState.h"
#include "representation/LevelState.h"
#include "logic/Score.h"
#include <iostream>
#include <cctype>
#include <map>

VictoryState::VictoryState(int nextLevelNum, int score)
    : fontLoaded(false)
    , continueRequested(false)
    , menuRequested(false)
    , spaceWasPressed(false)
    , mWasPressed(false)
    , nextLevel(nextLevelNum)
    , finalScore(score)
    , isGameOver(nextLevelNum == 0)
    , qualifiesForHighScore(false)
    , isEnteringName(false)
    , nameSubmitted(false)
    , playerNameInput("")
{
  std::cout << "\n🎉 [VICTORYSTATE] CONSTRUCTOR CALLED!" << std::endl;
  std::cout << "    Next Level: " << nextLevel << std::endl;
  std::cout << "    Final Score: " << finalScore << std::endl;

  // ✅ Check if score qualifies for high score
  if (score > 0) {
    Score tempScore;
    tempScore.loadHighScores();
    qualifiesForHighScore = tempScore.isHighScore(score);

    if (qualifiesForHighScore) {
      std::cout << "[VictoryState] Score qualifies for top 5! Will ask for name." << std::endl;
      isEnteringName = true;  // Start in name entry mode
    }
  }

  if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
    fontLoaded = true;
    std::cout << "[VictoryState] ✅ Font loaded successfully" << std::endl;
  } else {
    std::cerr << "[VictoryState] ❌ FAILED to load font!" << std::endl;
  }

  setupTexts();
  if (qualifiesForHighScore) {
    setupNameInput();
  }
}

void VictoryState::setupTexts() {
  if (!fontLoaded) return;

  // Title
  titleText.setFont(font);
  if (isGameOver) {
    titleText.setString("GAME OVER");
    titleText.setFillColor(sf::Color::Red);
  } else {
    titleText.setString("LEVEL CLEARED!");
    titleText.setFillColor(sf::Color::Yellow);
  }
  titleText.setCharacterSize(40);

  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
  titleText.setPosition(400.f, 200.f);

  // Score
  scoreText.setFont(font);
  scoreText.setString("Score: " + std::to_string(finalScore));
  scoreText.setCharacterSize(24);
  scoreText.setFillColor(sf::Color::White);

  sf::FloatRect scoreBounds = scoreText.getLocalBounds();
  scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
  scoreText.setPosition(400.f, 280.f);

  // Continue text
  continueText.setFont(font);
  if (isGameOver) {
    continueText.setString("");
  } else {
    continueText.setString("Press SPACE for Next Level");
    continueText.setCharacterSize(16);
    continueText.setFillColor(sf::Color::Green);

    sf::FloatRect contBounds = continueText.getLocalBounds();
    continueText.setOrigin(contBounds.width / 2.f, contBounds.height / 2.f);
    continueText.setPosition(400.f, 550.f);
  }

  // Menu text
  menuText.setFont(font);
  menuText.setString("Press M for Menu");
  menuText.setCharacterSize(16);
  menuText.setFillColor(sf::Color::Cyan);

  sf::FloatRect menuBounds = menuText.getLocalBounds();
  menuText.setOrigin(menuBounds.width / 2.f, menuBounds.height / 2.f);
  menuText.setPosition(400.f, 600.f);
}

// ✅ NEW: Setup name input UI
void VictoryState::setupNameInput() {
  if (!fontLoaded) return;

  // Prompt text
  namePromptText.setFont(font);
  namePromptText.setString("NEW HIGH SCORE!\nEnter your name:");
  namePromptText.setCharacterSize(20);
  namePromptText.setFillColor(sf::Color::Yellow);
  namePromptText.setLineSpacing(1.3f);

  sf::FloatRect promptBounds = namePromptText.getLocalBounds();
  namePromptText.setOrigin(promptBounds.width / 2.f, promptBounds.height / 2.f);
  namePromptText.setPosition(400.f, 360.f);

  // Input box
  nameInputBox.setSize(sf::Vector2f(300.f, 50.f));
  nameInputBox.setPosition((800.f - 300.f) / 2.f, 430.f);
  nameInputBox.setFillColor(sf::Color(30, 30, 30));
  nameInputBox.setOutlineColor(sf::Color::Yellow);
  nameInputBox.setOutlineThickness(3.f);

  // Input text
  nameInputText.setFont(font);
  nameInputText.setString("");
  nameInputText.setCharacterSize(24);
  nameInputText.setFillColor(sf::Color::White);
  nameInputText.setPosition(260.f, 442.f);
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
  // ✅ Handle name input first if we're entering a high score
  if (isEnteringName && !nameSubmitted) {
    // Handle backspace
    static bool backspaceWasPressed = false;
    bool backspaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace);

    if (backspaceIsPressed && !backspaceWasPressed) {
      if (!playerNameInput.empty()) {
        playerNameInput.pop_back();
        nameInputText.setString(playerNameInput);
      }
    }
    backspaceWasPressed = backspaceIsPressed;

    // Handle letter/number input (A-Z, 0-9)
    // Only accept input if name is less than 10 characters
    bool textChanged = false;

    if (playerNameInput.length() < 10) {
      static std::map<sf::Keyboard::Key, char> keyMap;
      static bool keyMapInitialized = false;

      if (!keyMapInitialized) {
        // Initialize key map once
        keyMap[sf::Keyboard::A] = 'A'; keyMap[sf::Keyboard::B] = 'B';
        keyMap[sf::Keyboard::C] = 'C'; keyMap[sf::Keyboard::D] = 'D';
        keyMap[sf::Keyboard::E] = 'E'; keyMap[sf::Keyboard::F] = 'F';
        keyMap[sf::Keyboard::G] = 'G'; keyMap[sf::Keyboard::H] = 'H';
        keyMap[sf::Keyboard::I] = 'I'; keyMap[sf::Keyboard::J] = 'J';
        keyMap[sf::Keyboard::K] = 'K'; keyMap[sf::Keyboard::L] = 'L';
        keyMap[sf::Keyboard::M] = 'M'; keyMap[sf::Keyboard::N] = 'N';
        keyMap[sf::Keyboard::O] = 'O'; keyMap[sf::Keyboard::P] = 'P';
        keyMap[sf::Keyboard::Q] = 'Q'; keyMap[sf::Keyboard::R] = 'R';
        keyMap[sf::Keyboard::S] = 'S'; keyMap[sf::Keyboard::T] = 'T';
        keyMap[sf::Keyboard::U] = 'U'; keyMap[sf::Keyboard::V] = 'V';
        keyMap[sf::Keyboard::W] = 'W'; keyMap[sf::Keyboard::X] = 'X';
        keyMap[sf::Keyboard::Y] = 'Y'; keyMap[sf::Keyboard::Z] = 'Z';
        keyMap[sf::Keyboard::Num0] = '0'; keyMap[sf::Keyboard::Num1] = '1';
        keyMap[sf::Keyboard::Num2] = '2'; keyMap[sf::Keyboard::Num3] = '3';
        keyMap[sf::Keyboard::Num4] = '4'; keyMap[sf::Keyboard::Num5] = '5';
        keyMap[sf::Keyboard::Num6] = '6'; keyMap[sf::Keyboard::Num7] = '7';
        keyMap[sf::Keyboard::Num8] = '8'; keyMap[sf::Keyboard::Num9] = '9';
        keyMapInitialized = true;
      }

      // ✅ FIXED: Track all keys and update them all each frame
      static std::map<sf::Keyboard::Key, bool> keyWasPressed;

      // Check each key and only add character if key was just pressed
      for (const auto& pair : keyMap) {
        bool isPressed = sf::Keyboard::isKeyPressed(pair.first);

        // ✅ Key just pressed (wasn't pressed last frame, is pressed now)
        if (isPressed && !keyWasPressed[pair.first]) {
          playerNameInput += pair.second;
          textChanged = true;
          std::cout << "[VictoryState] Name: " << playerNameInput << std::endl;
        }

        // ✅ Update state for this key AFTER checking
        keyWasPressed[pair.first] = isPressed;
      }
    }

    // ✅ Update text display every frame (ensures it always shows correctly)
    nameInputText.setString(playerNameInput);

    // Check for Enter key to submit
    static bool enterWasPressed = false;
    bool enterIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);

    if (enterIsPressed && !enterWasPressed && !playerNameInput.empty()) {
      submitHighScore();
    }
    enterWasPressed = enterIsPressed;

    return;  // Don't process other controls while entering name
  }

  // Normal victory screen controls
  bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
  bool mIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);

  // Continue to next level
  if (!isGameOver && spaceIsPressed && !spaceWasPressed) {
    if (stateManager) {
      std::cout << "[VictoryState] Starting level " << nextLevel << "..." << std::endl;
      stateManager->clearAndPushState(std::make_unique<LevelState>(nextLevel, finalScore));
    }
  }

  // Return to menu
  if (mIsPressed && !mWasPressed) {
    if (stateManager) {
      std::cout << "[VictoryState] Returning to menu..." << std::endl;
      stateManager->clearAndPushState(std::make_unique<MenuState>());
    }
  }

  spaceWasPressed = spaceIsPressed;
  mWasPressed = mIsPressed;
}

// ✅ NEW: Handle text input for name (simplified version using key states)
void VictoryState::handleTextInput(sf::Uint32 unicode) {
  // This method is kept for potential future use but simplified version uses keys directly
}

// ✅ NEW: Submit the high score with player name
void VictoryState::submitHighScore() {
  if (playerNameInput.empty()) {
    playerNameInput = "PLAYER";
  }

  Score tempScore;
  tempScore.loadHighScores();
  tempScore.addHighScore(playerNameInput, finalScore);  // ✅ Now includes name

  nameSubmitted = true;
  isEnteringName = false;

  std::cout << "[VictoryState] High score saved: " << playerNameInput
            << " - " << finalScore << std::endl;

  // ✅ NEW: If game over, automatically return to menu
  if (isGameOver && stateManager) {
    std::cout << "[VictoryState] Game over - returning to menu automatically..." << std::endl;
    stateManager->clearAndPushState(std::make_unique<MenuState>());
  }
}

void VictoryState::update(float deltaTime) {
  // Nothing to update
}

void VictoryState::render(sf::RenderWindow& window) {
  if (!fontLoaded) return;

  // Draw overlay
  sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
  overlay.setFillColor(sf::Color(0, 0, 0, 200));
  window.draw(overlay);

  // Draw title and score
  window.draw(titleText);
  window.draw(scoreText);

  // ✅ If entering name, show name input UI
  if (isEnteringName && !nameSubmitted) {
    window.draw(namePromptText);
    window.draw(nameInputBox);
    window.draw(nameInputText);

    // Show hint
    sf::Text hintText;
    hintText.setFont(font);
    hintText.setString("Use keyboard - Press ENTER when done");
    hintText.setCharacterSize(12);
    hintText.setFillColor(sf::Color(150, 150, 150));

    sf::FloatRect hintBounds = hintText.getLocalBounds();
    hintText.setOrigin(hintBounds.width / 2.f, hintBounds.height / 2.f);
    hintText.setPosition(400.f, 510.f);
    window.draw(hintText);
  } else {
    // Show normal victory screen buttons
    // ✅ Don't show controls if we just submitted high score on game over (auto-transitioning)
    if (!(nameSubmitted && isGameOver)) {
      if (!isGameOver) {
        window.draw(continueText);
      }
      window.draw(menuText);
    }
  }
}