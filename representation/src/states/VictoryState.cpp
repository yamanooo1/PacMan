#include "../../include/representation/states/VictoryState.h"
#include "../../../logic/include/logic/systems/Score.h"
#include "../../include/representation/states/LevelState.h"
#include "../../include/representation/states/MenuState.h"
#include "../../include/representation/states/StateManager.h"
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
    , windowWidth(800.0f)
    , windowHeight(860.0f)
{
  if (isGameOver && score > 0) {
    Score tempScore;
    tempScore.loadHighScores();
    qualifiesForHighScore = tempScore.isHighScore(score);

    if (qualifiesForHighScore) {
      isEnteringName = true;
    }
  }

  if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
    fontLoaded = true;
  }

  setupTexts();
  if (qualifiesForHighScore && isGameOver) {
    setupNameInput();
  }
}

void VictoryState::onWindowResize(float width, float height) {
  windowWidth = width;
  windowHeight = height;

  setupTexts();
  if (qualifiesForHighScore && isGameOver) {
    setupNameInput();
  }
}

void VictoryState::setupTexts() {
  if (!fontLoaded) return;

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
  titleText.setPosition(windowWidth * 0.5f, windowHeight * 0.233f);

  scoreText.setFont(font);
  scoreText.setString("Score: " + std::to_string(finalScore));
  scoreText.setCharacterSize(24);
  scoreText.setFillColor(sf::Color::White);

  sf::FloatRect scoreBounds = scoreText.getLocalBounds();
  scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
  scoreText.setPosition(windowWidth * 0.5f, windowHeight * 0.326f);

  continueText.setFont(font);
  if (isGameOver) {
    continueText.setString("");
  } else {
    continueText.setString("Press SPACE for Next Level");
    continueText.setCharacterSize(16);
    continueText.setFillColor(sf::Color::Green);

    sf::FloatRect contBounds = continueText.getLocalBounds();
    continueText.setOrigin(contBounds.width / 2.f, contBounds.height / 2.f);
    continueText.setPosition(windowWidth * 0.5f, windowHeight * 0.64f);
  }

  menuText.setFont(font);
  menuText.setString("Press M for Menu");
  menuText.setCharacterSize(16);
  menuText.setFillColor(sf::Color::Cyan);

  sf::FloatRect menuBounds = menuText.getLocalBounds();
  menuText.setOrigin(menuBounds.width / 2.f, menuBounds.height / 2.f);
  menuText.setPosition(windowWidth * 0.5f, windowHeight * 0.698f);
}

void VictoryState::setupNameInput() {
  if (!fontLoaded) return;

  namePromptText.setFont(font);
  namePromptText.setString("NEW HIGH SCORE!\nEnter your name:");
  namePromptText.setCharacterSize(20);
  namePromptText.setFillColor(sf::Color::Yellow);
  namePromptText.setLineSpacing(1.3f);

  sf::FloatRect promptBounds = namePromptText.getLocalBounds();
  namePromptText.setOrigin(promptBounds.width / 2.f, promptBounds.height / 2.f);
  namePromptText.setPosition(windowWidth * 0.5f, windowHeight * 0.419f);

  float boxWidth = windowWidth * 0.375f;
  float boxHeight = windowHeight * 0.058f;

  nameInputBox.setSize(sf::Vector2f(boxWidth, boxHeight));
  nameInputBox.setPosition((windowWidth - boxWidth) / 2.f, windowHeight * 0.5f);
  nameInputBox.setFillColor(sf::Color(30, 30, 30));
  nameInputBox.setOutlineColor(sf::Color::Yellow);
  nameInputBox.setOutlineThickness(3.f);

  nameInputText.setFont(font);
  nameInputText.setString("");
  nameInputText.setCharacterSize(24);
  nameInputText.setFillColor(sf::Color::White);
  nameInputText.setPosition(
    nameInputBox.getPosition().x + (boxWidth * 0.05f),
    nameInputBox.getPosition().y + (boxHeight * 0.2f)
  );
}

void VictoryState::onEnter() {
}

void VictoryState::onExit() {
}

void VictoryState::handleEvents(sf::RenderWindow& window) {
  if (isEnteringName && !nameSubmitted) {
    static bool backspaceWasPressed = false;
    bool backspaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace);

    if (backspaceIsPressed && !backspaceWasPressed) {
      if (!playerNameInput.empty()) {
        playerNameInput.pop_back();
        nameInputText.setString(playerNameInput);
      }
    }
    backspaceWasPressed = backspaceIsPressed;

    if (playerNameInput.length() < 10) {
      static std::map<sf::Keyboard::Key, char> keyMap;
      static bool keyMapInitialized = false;

      if (!keyMapInitialized) {
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

      static std::map<sf::Keyboard::Key, bool> keyWasPressed;

      for (const auto& pair : keyMap) {
        bool isPressed = sf::Keyboard::isKeyPressed(pair.first);

        if (isPressed && !keyWasPressed[pair.first]) {
          playerNameInput += pair.second;
        }

        keyWasPressed[pair.first] = isPressed;
      }
    }

    nameInputText.setString(playerNameInput);

    static bool enterWasPressed = false;
    bool enterIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);

    if (enterIsPressed && !enterWasPressed && !playerNameInput.empty()) {
      submitHighScore();
    }
    enterWasPressed = enterIsPressed;

    return;
  }

  bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
  bool mIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);

  if (!isGameOver && spaceIsPressed && !spaceWasPressed) {
    if (stateManager) {
      stateManager->clearAndPushState(std::make_unique<LevelState>(nextLevel, finalScore));
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

void VictoryState::handleTextInput(sf::Uint32 unicode) {
}

void VictoryState::submitHighScore() {
  if (playerNameInput.empty()) {
    playerNameInput = "PLAYER";
  }

  Score tempScore;
  tempScore.loadHighScores();
  tempScore.addHighScore(playerNameInput, finalScore);

  nameSubmitted = true;
  isEnteringName = false;

  if (isGameOver && stateManager) {
    stateManager->clearAndPushState(std::make_unique<MenuState>());
  }
}

void VictoryState::update(float deltaTime) {
}

void VictoryState::render(sf::RenderWindow& window) {
  if (!fontLoaded) return;

  sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
  overlay.setFillColor(sf::Color(0, 0, 0, 200));
  window.draw(overlay);

  window.draw(titleText);
  window.draw(scoreText);

  if (isEnteringName && !nameSubmitted) {
    window.draw(namePromptText);
    window.draw(nameInputBox);
    window.draw(nameInputText);

    sf::Text hintText;
    hintText.setFont(font);
    hintText.setString("Use keyboard - Press ENTER when done");
    hintText.setCharacterSize(12);
    hintText.setFillColor(sf::Color(150, 150, 150));

    sf::FloatRect hintBounds = hintText.getLocalBounds();
    hintText.setOrigin(hintBounds.width / 2.f, hintBounds.height / 2.f);
    hintText.setPosition(windowWidth * 0.5f, windowHeight * 0.593f);
    window.draw(hintText);
  } else {
    if (!(nameSubmitted && isGameOver)) {
      if (!isGameOver) {
        window.draw(continueText);
      }
      window.draw(menuText);
    }
  }
}