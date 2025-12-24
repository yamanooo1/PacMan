#include "representation/states/MenuState.h"
#include "../../../logic/include/logic/systems/Score.h"
#include "representation/states/LevelState.h"
#include "representation/states/StateManager.h"
#include "representation/systems/SoundManager.h"

namespace representation {

/**
 * @brief Initialize MenuState with font and UI setup
 *
 * UI initialized with default 800×860 dimensions.
 * Window resize will adjust later if needed.
 */
MenuState::MenuState()
    : fontLoaded(false), isPlayButtonHovered(false), isExitButtonHovered(false), windowWidth(800.0f),
      windowHeight(860.0f) {
    if (font.loadFromFile("../../resources/fonts/font-emulogic/emulogic.ttf")) {
        fontLoaded = true;
    }

    setupTexts();
    setupPlayButton();
    setupExitButton();
    setupHowToPlay();
}

/**
 * @brief Rebuild UI for new window size
 *
 * All elements repositioned as % of window dimensions.
 */
void MenuState::onWindowResize(float width, float height) {
    windowWidth = width;
    windowHeight = height;

    setupTexts();
    setupPlayButton();
    setupExitButton();
    setupHowToPlay();
    setupLeaderboard();
}

/**
 * @brief Setup main title text
 *
 * Position: Centered horizontally, 6% from top
 */
void MenuState::setupTexts() {
    if (!fontLoaded)
        return;

    titleText.setFont(font);
    titleText.setString("PAC-MAN");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Yellow);

    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition((windowWidth - titleBounds.width) / 2.f, windowHeight * 0.06f);
}

/**
 * @brief Setup green Play button
 *
 * Size: 37.5% width × 7% height
 * Position: Centered, 79% from top
 * Colors: Dark green (60,100,60) → Light green (90,140,90) on hover
 */
void MenuState::setupPlayButton() {
    if (!fontLoaded)
        return;

    float buttonWidth = windowWidth * 0.375f;
    float buttonHeight = windowHeight * 0.07f;

    playButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    playButton.setFillColor(sf::Color(60, 100, 60));
    playButton.setOutlineColor(sf::Color::White);
    playButton.setOutlineThickness(3.f);

    playButton.setPosition((windowWidth - buttonWidth) / 2.f, windowHeight * 0.79f);

    playButtonText.setFont(font);
    playButtonText.setString("PLAY");
    playButtonText.setCharacterSize(28);
    playButtonText.setFillColor(sf::Color::White);

    // Center text in button (manual adjustment for pixel font)
    sf::FloatRect textBounds = playButtonText.getLocalBounds();
    playButtonText.setPosition(playButton.getPosition().x + (buttonWidth - textBounds.width) / 2.f - textBounds.left,
                               playButton.getPosition().y + (buttonHeight - textBounds.height) / 2.f - textBounds.top -
                                   5.f);
}

/**
 * @brief Setup red Exit button
 *
 * Size: 37.5% width × 7% height
 * Position: Centered, 88% from top
 * Colors: Dark red (140,60,60) → Light red (180,80,80) on hover
 */
void MenuState::setupExitButton() {
    if (!fontLoaded)
        return;

    float buttonWidth = windowWidth * 0.375f;
    float buttonHeight = windowHeight * 0.07f;

    exitButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    exitButton.setFillColor(sf::Color(140, 60, 60));
    exitButton.setOutlineColor(sf::Color::White);
    exitButton.setOutlineThickness(3.f);

    exitButton.setPosition((windowWidth - buttonWidth) / 2.f, windowHeight * 0.88f);

    exitButtonText.setFont(font);
    exitButtonText.setString("EXIT");
    exitButtonText.setCharacterSize(28);
    exitButtonText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = exitButtonText.getLocalBounds();
    exitButtonText.setPosition(exitButton.getPosition().x + (buttonWidth - textBounds.width) / 2.f - textBounds.left,
                               exitButton.getPosition().y + (buttonHeight - textBounds.height) / 2.f - textBounds.top -
                                   5.f);
}

/**
 * @brief Setup "How to Play" instructions
 *
 * Three text elements:
 * 1. Title (17.4% from top): "HOW TO PLAY"
 * 2. Controls (22% from top): Keyboard layout
 * 3. Points (30% from top): Scoring system
 *
 * Line spacing: 1.5× for readability
 */
void MenuState::setupHowToPlay() {
    if (!fontLoaded)
        return;

    howToPlayTitle.setFont(font);
    howToPlayTitle.setString("HOW TO PLAY");
    howToPlayTitle.setCharacterSize(20);
    howToPlayTitle.setFillColor(sf::Color::Cyan);

    sf::FloatRect titleBounds = howToPlayTitle.getLocalBounds();
    howToPlayTitle.setPosition((windowWidth - titleBounds.width) / 2.f, windowHeight * 0.174f);

    howToPlayControls.setFont(font);
    howToPlayControls.setString("CONTROLS:\n"
                                "Arrow Keys / WASD - Move\n"
                                "ESC - Pause   M - Menu");
    howToPlayControls.setCharacterSize(13);
    howToPlayControls.setFillColor(sf::Color::White);
    howToPlayControls.setLineSpacing(1.5f);

    sf::FloatRect controlsBounds = howToPlayControls.getLocalBounds();
    howToPlayControls.setPosition((windowWidth - controlsBounds.width) / 2.f, windowHeight * 0.22f);

    howToPlayPoints.setFont(font);
    howToPlayPoints.setString("POINTS:\n"
                              "Coin: 10-30   Fruit: 50\n"
                              "Ghost: 200   Level: 1000");
    howToPlayPoints.setCharacterSize(13);
    howToPlayPoints.setFillColor(sf::Color(255, 255, 150));
    howToPlayPoints.setLineSpacing(1.5f);

    sf::FloatRect pointsBounds = howToPlayPoints.getLocalBounds();
    howToPlayPoints.setPosition((windowWidth - pointsBounds.width) / 2.f, windowHeight * 0.30f);
}

/**
 * @brief State entry - reload leaderboard, stop music
 */
void MenuState::onEnter() {
    SoundManager& soundManager = SoundManager::getInstance();
    soundManager.stopMusic();
    soundManager.stopMovementSound();
    soundManager.stopFearModeSound();

    setupLeaderboard();
}

/**
 * @brief Load top 5 high scores from file
 *
 * Layout:
 * - Box: 75% width × 32.5% height at 41.8% from top
 * - Title: "TOP 5 SCORES" centered
 * - Scores: 5 rows with name (left) and score (right aligned)
 * - Empty slots: "N. ------" / "---" in gray
 *
 * Spacing: 5.23% vertical between rows
 */
void MenuState::setupLeaderboard() {
    if (!fontLoaded)
        return;

    // Load high scores from file
    logic::Score tempScore;
    tempScore.loadHighScores();
    auto highScores = tempScore.getHighScores();

    // Create yellow box background
    float boxWidth = windowWidth * 0.75f;
    float boxHeight = windowHeight * 0.325f;

    leaderboardBox.setSize(sf::Vector2f(boxWidth, boxHeight));
    leaderboardBox.setFillColor(sf::Color(255, 255, 0, 40)); // Transparent yellow
    leaderboardBox.setOutlineColor(sf::Color::Yellow);
    leaderboardBox.setOutlineThickness(4.f);
    leaderboardBox.setPosition((windowWidth - boxWidth) / 2.f, windowHeight * 0.418f);

    // Title text
    leaderboardTitle.setFont(font);
    leaderboardTitle.setString("TOP 5 SCORES");
    leaderboardTitle.setCharacterSize(24);
    leaderboardTitle.setFillColor(sf::Color::Yellow);

    sf::FloatRect titleBounds = leaderboardTitle.getLocalBounds();
    leaderboardTitle.setPosition((windowWidth - titleBounds.width) / 2.f, windowHeight * 0.442f);

    // Clear previous score texts
    scoreNameTexts.clear();
    scoreValueTexts.clear();

    // Position for score entries
    float startY = windowHeight * 0.5f;
    float leftX = windowWidth * 0.1875f;  // 18.75% from left
    float rightX = windowWidth * 0.8125f; // 81.25% from left (right align)
    float spacing = windowHeight * 0.0523f;

    // Create 5 score text pairs
    for (size_t i = 0; i < 5; ++i) {
        float yPosition = startY + (i * spacing);

        // Name text (left aligned)
        sf::Text nameText;
        nameText.setFont(font);
        nameText.setCharacterSize(16);

        if (i < highScores.size()) {
            nameText.setString(std::to_string(i + 1) + ". " + highScores[i].playerName);
            nameText.setFillColor(sf::Color::White);
        } else {
            nameText.setString(std::to_string(i + 1) + ". ------");
            nameText.setFillColor(sf::Color(100, 100, 100)); // Gray for empty
        }

        nameText.setPosition(leftX, yPosition);
        scoreNameTexts.push_back(nameText);

        // Score value (right aligned)
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

        // Right align by subtracting text width
        sf::FloatRect scoreBounds = scoreText.getLocalBounds();
        scoreText.setPosition(rightX - scoreBounds.width, yPosition);
        scoreValueTexts.push_back(scoreText);
    }
}

/**
 * @brief Handle mouse clicks on buttons
 *
 * Input handling:
 * - Static mouseWasPressed prevents double-clicks
 * - Check hover state on click release
 * - Play button → push LevelState(1)
 * - Exit button → close window
 */
void MenuState::handleEvents(sf::RenderWindow& window) {
    static bool mouseWasPressed = false;
    bool mouseIsPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (mouseIsPressed && !mouseWasPressed) {
        if (isMouseOverButton(playButton, window)) {
            if (stateManager) {
                stateManager->pushState(std::make_unique<LevelState>(1));
            }
        }

        if (isMouseOverButton(exitButton, window)) {
            window.close();
        }
    }

    mouseWasPressed = mouseIsPressed;
}

void MenuState::update(float deltaTime) {}

/**
 * @brief Check if mouse is inside button bounds
 *
 * @return true if mouse position within button rectangle
 */
bool MenuState::isMouseOverButton(const sf::RectangleShape& button, sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosFloat(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return button.getGlobalBounds().contains(mousePosFloat);
}

/**
 * @brief Render all menu elements
 *
 * Rendering order:
 * 1. Title
 * 2. How to Play section
 * 3. Leaderboard box + entries
 * 4. Buttons (with hover effects)
 *
 * Hover effects applied per frame based on mouse position.
 */
void MenuState::render(sf::RenderWindow& window) {
    if (!fontLoaded)
        return;

    // Update hover states
    isPlayButtonHovered = isMouseOverButton(playButton, window);
    isExitButtonHovered = isMouseOverButton(exitButton, window);

    // Apply hover colors
    if (isPlayButtonHovered) {
        playButton.setFillColor(sf::Color(90, 140, 90));
    } else {
        playButton.setFillColor(sf::Color(60, 100, 60));
    }

    if (isExitButtonHovered) {
        exitButton.setFillColor(sf::Color(180, 80, 80));
    } else {
        exitButton.setFillColor(sf::Color(140, 60, 60));
    }

    // Draw all elements
    window.draw(titleText);

    window.draw(howToPlayTitle);
    window.draw(howToPlayControls);
    window.draw(howToPlayPoints);

    window.draw(leaderboardBox);
    window.draw(leaderboardTitle);

    for (const auto& nameText : scoreNameTexts) {
        window.draw(nameText);
    }
    for (const auto& scoreText : scoreValueTexts) {
        window.draw(scoreText);
    }

    window.draw(playButton);
    window.draw(playButtonText);

    window.draw(exitButton);
    window.draw(exitButtonText);
}

} // namespace representation