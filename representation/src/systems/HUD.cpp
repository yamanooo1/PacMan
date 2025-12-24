#include "../../include/representation/systems/HUD.h"
#include "../../../logic/include/logic/core/World.h"
#include "../../../logic/include/logic/systems/Lives.h"
#include "../../../logic/include/logic/systems/Score.h"

namespace representation {

HUD::HUD(sf::RenderWindow& win, int height) : window(win), hudHeight(height), fontLoaded(false) {}

/**
 * @brief Load font for HUD text rendering
 */
void HUD::loadFont(const std::string& fontPath) {
    if (font.loadFromFile(fontPath)) {
        fontLoaded = true;
    }
}

/**
 * @brief Render HUD bar with game stats
 *
 * Layout calculation:
 * - HUD Y position: windowHeight - hudHeight
 * - Score: 20px from left
 * - Level: Centered (window width / 2 - text width / 2)
 * - Lives: Right aligned (window width - text width - 20px)
 */
void HUD::draw(logic::World* world, logic::Score* score, logic::Lives* lives, int currentLevel) {
    if (!score || !lives)
        return;

    float windowHeight = static_cast<float>(window.getSize().y);
    float hudYPosition = windowHeight - hudHeight;

    // Black background bar
    sf::RectangleShape hudBackground;
    hudBackground.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(hudHeight)));
    hudBackground.setPosition(0.f, hudYPosition);
    hudBackground.setFillColor(sf::Color::Black);
    window.draw(hudBackground);

    if (!fontLoaded) {
        return;
    }

    int currentScore = score->getScore();
    int currentLives = lives->getLives();

    // Score text (left)
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("SCORE: " + std::to_string(currentScore));
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, hudYPosition + 20.f);
    window.draw(scoreText);

    // Lives text (right)
    sf::Text livesText;
    livesText.setFont(font);
    livesText.setString("LIVES: " + std::to_string(currentLives));
    livesText.setCharacterSize(20);
    livesText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = livesText.getLocalBounds();
    livesText.setPosition(window.getSize().x - textBounds.width - 20.f, hudYPosition + 20.f);
    window.draw(livesText);

    // Level text (center)
    sf::Text levelText;
    levelText.setFont(font);
    levelText.setString("LEVEL " + std::to_string(currentLevel));
    levelText.setCharacterSize(20);
    levelText.setFillColor(sf::Color::Yellow);

    sf::FloatRect levelBounds = levelText.getLocalBounds();
    float centerX = (window.getSize().x - levelBounds.width) / 2.f;
    levelText.setPosition(centerX, hudYPosition + 20.f);
    window.draw(levelText);
}

/**
 * @brief Draw "READY!" text at maze center
 *
 * Grid position (10.5, 10.4) calculated as:
 * - screenX = 10.5 × (gameWidth / 21.0)
 * - screenY = 10.4 × (gameHeight / 21.0)
 *
 * Game dimensions exclude HUD height.
 */
void HUD::drawReadyText(logic::World* world) {
    if (!world || !fontLoaded)
        return;
    if (!world->isReadyStateActive())
        return;

    sf::Text readyText;
    readyText.setFont(font);
    readyText.setString("READY!");
    readyText.setCharacterSize(24);
    readyText.setFillColor(sf::Color::Yellow);

    // Grid coordinates
    float gridX = 10.5f;
    float gridY = 10.4f;

    // Convert to screen space (game area only, exclude HUD)
    float gameHeight = static_cast<float>(window.getSize().y) - hudHeight;
    float gameWidth = static_cast<float>(window.getSize().x);

    float cellWidth = gameWidth / 21.0f;
    float cellHeight = gameHeight / 21.0f;

    float screenX = gridX * cellWidth;
    float screenY = gridY * cellHeight;

    // Center text on position
    sf::FloatRect textBounds = readyText.getLocalBounds();
    readyText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
    readyText.setPosition(screenX, screenY);

    window.draw(readyText);
}

/**
 * @brief Draw "LEVEL CLEARED!" text at maze center
 *
 * Similar to READY! but slightly lower (10.5 vs 10.4).
 * Green color to indicate success.
 */
void HUD::drawLevelClearedText(logic::World* world) {
    if (!world || !fontLoaded)
        return;
    if (!world->isLevelClearedDisplayActive())
        return;

    sf::Text clearedText;
    clearedText.setFont(font);
    clearedText.setString("LEVEL CLEARED!");
    clearedText.setCharacterSize(30);
    clearedText.setFillColor(sf::Color::Green);

    float gridX = 10.5f;
    float gridY = 10.5f;

    float gameHeight = static_cast<float>(window.getSize().y) - hudHeight;
    float gameWidth = static_cast<float>(window.getSize().x);

    float cellWidth = gameWidth / 21.0f;
    float cellHeight = gameHeight / 21.0f;

    float screenX = gridX * cellWidth;
    float screenY = gridY * cellHeight;

    sf::FloatRect textBounds = clearedText.getLocalBounds();
    clearedText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
    clearedText.setPosition(screenX, screenY);

    window.draw(clearedText);
}

} // namespace representation