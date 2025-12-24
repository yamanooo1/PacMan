#include "../../include/representation/states/VictoryState.h"
#include "../../../logic/include/logic/systems/Score.h"
#include "../../include/representation/states/LevelState.h"
#include "../../include/representation/states/MenuState.h"
#include "../../include/representation/states/StateManager.h"
#include <cctype>
#include <map>

namespace representation {

/**
 * @brief Initialize VictoryState and check high score qualification
 *
 * Qualification check:
 * - Only for game over (nextLevel == 0)
 * - Only if score > 0
 * - Checks if score beats 5th place
 * - Enables name input if qualified
 */
VictoryState::VictoryState(int nextLevelNum, int score)
    : fontLoaded(false), continueRequested(false), menuRequested(false), spaceWasPressed(false), mWasPressed(false),
      nextLevel(nextLevelNum), finalScore(score), isGameOver(nextLevelNum == 0), qualifiesForHighScore(false),
      isEnteringName(false), nameSubmitted(false), playerNameInput(""), windowWidth(800.0f), windowHeight(860.0f) {

    // Check high score qualification
    if (isGameOver && score > 0) {
        logic::Score tempScore;
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

/**
 * @brief Setup title, score, and action texts
 *
 * Title varies by mode:
 * - Level complete: "LEVEL CLEARED!" (yellow)
 * - Game over: "GAME OVER" (red)
 *
 * Positions (% from top):
 * - Title: 23.3%
 * - Score: 32.6%
 * - Continue: 64% (level complete only)
 * - Menu: 69.8%
 */
void VictoryState::setupTexts() {
    if (!fontLoaded)
        return;

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
        continueText.setString(""); // No continue on game over
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

/**
 * @brief Setup name input UI for high score entry
 *
 * Components:
 * - Prompt: "NEW HIGH SCORE!\nEnter your name:" (41.9% from top)
 * - Input box: Yellow outlined (50% from top, 37.5% width × 5.8% height)
 * - Input text: Display user typing (inside box)
 * - Hint: "Use keyboard - Press ENTER when done" (59.3% from top)
 */
void VictoryState::setupNameInput() {
    if (!fontLoaded)
        return;

    namePromptText.setFont(font);
    namePromptText.setString("NEW HIGH SCORE!\nEnter your name:");
    namePromptText.setCharacterSize(20);
    namePromptText.setFillColor(sf::Color::Yellow);
    namePromptText.setLineSpacing(1.3f);

    sf::FloatRect promptBounds = namePromptText.getLocalBounds();
    namePromptText.setOrigin(promptBounds.width / 2.f, promptBounds.height / 2.f);
    namePromptText.setPosition(windowWidth * 0.5f, windowHeight * 0.419f);

    // Input box
    float boxWidth = windowWidth * 0.375f;
    float boxHeight = windowHeight * 0.058f;

    nameInputBox.setSize(sf::Vector2f(boxWidth, boxHeight));
    nameInputBox.setPosition((windowWidth - boxWidth) / 2.f, windowHeight * 0.5f);
    nameInputBox.setFillColor(sf::Color(30, 30, 30));
    nameInputBox.setOutlineColor(sf::Color::Yellow);
    nameInputBox.setOutlineThickness(3.f);

    // Input text (empty initially)
    nameInputText.setFont(font);
    nameInputText.setString("");
    nameInputText.setCharacterSize(24);
    nameInputText.setFillColor(sf::Color::White);
    nameInputText.setPosition(nameInputBox.getPosition().x + (boxWidth * 0.05f),
                              nameInputBox.getPosition().y + (boxHeight * 0.2f));
}

void VictoryState::onEnter() {}

void VictoryState::onExit() {}

/**
 * @brief Handle name input or navigation keys
 *
 * Two modes:
 *
 * 1. **Name Entry Mode** (isEnteringName && !nameSubmitted):
 *    - A-Z: Append character (max 10)
 *    - 0-9: Append digit (max 10)
 *    - Backspace: Remove last character
 *    - Enter: Submit name (if not empty)
 *    - All keys use static tracking
 *
 * 2. **Navigation Mode**:
 *    - SPACE: Next level (if level complete)
 *    - M: Return to menu
 *
 * Key Mapping:
 * - Static map initialized once
 * - Maps sf::Keyboard::Key → char
 * - Uppercase only (A-Z, 0-9)
 */
void VictoryState::handleEvents(sf::RenderWindow& window) {
    if (isEnteringName && !nameSubmitted) {
        // Backspace handling
        static bool backspaceWasPressed = false;
        bool backspaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace);

        if (backspaceIsPressed && !backspaceWasPressed) {
            if (!playerNameInput.empty()) {
                playerNameInput.pop_back();
                nameInputText.setString(playerNameInput);
            }
        }
        backspaceWasPressed = backspaceIsPressed;

        // Character input (A-Z, 0-9)
        if (playerNameInput.length() < 10) {
            static std::map<sf::Keyboard::Key, char> keyMap;
            static bool keyMapInitialized = false;

            // Initialize key mapping once
            if (!keyMapInitialized) {
                keyMap[sf::Keyboard::A] = 'A';
                keyMap[sf::Keyboard::B] = 'B';
                keyMap[sf::Keyboard::C] = 'C';
                keyMap[sf::Keyboard::D] = 'D';
                keyMap[sf::Keyboard::E] = 'E';
                keyMap[sf::Keyboard::F] = 'F';
                keyMap[sf::Keyboard::G] = 'G';
                keyMap[sf::Keyboard::H] = 'H';
                keyMap[sf::Keyboard::I] = 'I';
                keyMap[sf::Keyboard::J] = 'J';
                keyMap[sf::Keyboard::K] = 'K';
                keyMap[sf::Keyboard::L] = 'L';
                keyMap[sf::Keyboard::M] = 'M';
                keyMap[sf::Keyboard::N] = 'N';
                keyMap[sf::Keyboard::O] = 'O';
                keyMap[sf::Keyboard::P] = 'P';
                keyMap[sf::Keyboard::Q] = 'Q';
                keyMap[sf::Keyboard::R] = 'R';
                keyMap[sf::Keyboard::S] = 'S';
                keyMap[sf::Keyboard::T] = 'T';
                keyMap[sf::Keyboard::U] = 'U';
                keyMap[sf::Keyboard::V] = 'V';
                keyMap[sf::Keyboard::W] = 'W';
                keyMap[sf::Keyboard::X] = 'X';
                keyMap[sf::Keyboard::Y] = 'Y';
                keyMap[sf::Keyboard::Z] = 'Z';
                keyMap[sf::Keyboard::Num0] = '0';
                keyMap[sf::Keyboard::Num1] = '1';
                keyMap[sf::Keyboard::Num2] = '2';
                keyMap[sf::Keyboard::Num3] = '3';
                keyMap[sf::Keyboard::Num4] = '4';
                keyMap[sf::Keyboard::Num5] = '5';
                keyMap[sf::Keyboard::Num6] = '6';
                keyMap[sf::Keyboard::Num7] = '7';
                keyMap[sf::Keyboard::Num8] = '8';
                keyMap[sf::Keyboard::Num9] = '9';
                keyMapInitialized = true;
            }

            // Track each key separately
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

        // Enter to submit
        static bool enterWasPressed = false;
        bool enterIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);

        if (enterIsPressed && !enterWasPressed && !playerNameInput.empty()) {
            submitHighScore();
        }
        enterWasPressed = enterIsPressed;

        return; // Skip navigation while entering name
    }

    // Navigation mode
    bool spaceIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    bool mIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);

    // SPACE for next level (level complete only)
    if (!isGameOver && spaceIsPressed && !spaceWasPressed) {
        if (stateManager) {
            stateManager->clearAndPushState(std::make_unique<LevelState>(nextLevel, finalScore));
        }
    }

    // M for menu
    if (mIsPressed && !mWasPressed) {
        if (stateManager) {
            stateManager->clearAndPushState(std::make_unique<MenuState>());
        }
    }

    spaceWasPressed = spaceIsPressed;
    mWasPressed = mIsPressed;
}

void VictoryState::handleTextInput(sf::Uint32 unicode) {}

/**
 * @brief Submit name and score to leaderboard
 *
 * Process:
 * 1. Default empty name to "PLAYER"
 * 2. Load current high scores
 * 3. Add new entry
 * 4. Save to file (sorted top 5)
 * 5. Return to menu
 */
void VictoryState::submitHighScore() {
    if (playerNameInput.empty()) {
        playerNameInput = "PLAYER";
    }

    logic::Score tempScore;
    tempScore.loadHighScores();
    tempScore.addHighScore(playerNameInput, finalScore);

    nameSubmitted = true;
    isEnteringName = false;

    // Auto-transition to menu after submission
    if (isGameOver && stateManager) {
        stateManager->clearAndPushState(std::make_unique<MenuState>());
    }
}

void VictoryState::update(float deltaTime) {}

/**
 * @brief Render victory/game over screen
 *
 * Rendering logic:
 * 1. Dark overlay (alpha 200)
 * 2. Title + score (always)
 * 3. If entering name:
 *    - Name prompt
 *    - Input box + text
 *    - Hint text
 * 4. Else (navigation):
 *    - Continue text (level complete only)
 *    - Menu text
 */
void VictoryState::render(sf::RenderWindow& window) {
    if (!fontLoaded)
        return;

    // Dark overlay
    sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    window.draw(titleText);
    window.draw(scoreText);

    if (isEnteringName && !nameSubmitted) {
        // High score entry UI
        window.draw(namePromptText);
        window.draw(nameInputBox);
        window.draw(nameInputText);

        // Hint text
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
        // Navigation UI (skip if just submitted on game over)
        if (!(nameSubmitted && isGameOver)) {
            if (!isGameOver) {
                window.draw(continueText);
            }
            window.draw(menuText);
        }
    }
}

} // namespace representation