#include "../../include/representation/systems/HUD.h"
#include "../../../logic/include/logic/core/World.h"
#include "../../../logic/include/logic/systems/Lives.h"
#include "../../../logic/include/logic/systems/Score.h"

HUD::HUD(sf::RenderWindow& win, int height) : window(win), hudHeight(height), fontLoaded(false) {}

void HUD::loadFont(const std::string& fontPath) {
    if (font.loadFromFile(fontPath)) {
        fontLoaded = true;
    }
}

void HUD::draw(World* world, Score* score, Lives* lives, int currentLevel) {
    if (!score || !lives)
        return;

    float windowHeight = static_cast<float>(window.getSize().y);
    float hudYPosition = windowHeight - hudHeight;

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

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("SCORE: " + std::to_string(currentScore));
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, hudYPosition + 20.f);
    window.draw(scoreText);

    sf::Text livesText;
    livesText.setFont(font);
    livesText.setString("LIVES: " + std::to_string(currentLives));
    livesText.setCharacterSize(20);
    livesText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = livesText.getLocalBounds();
    livesText.setPosition(window.getSize().x - textBounds.width - 20.f, hudYPosition + 20.f);
    window.draw(livesText);

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

void HUD::drawReadyText(World* world) {
    if (!world || !fontLoaded)
        return;
    if (!world->isReadyStateActive())
        return;

    sf::Text readyText;
    readyText.setFont(font);
    readyText.setString("READY!");
    readyText.setCharacterSize(24);
    readyText.setFillColor(sf::Color::Yellow);

    float gridX = 10.5f;
    float gridY = 10.4f;

    float gameHeight = static_cast<float>(window.getSize().y) - hudHeight;
    float gameWidth = static_cast<float>(window.getSize().x);

    float cellWidth = gameWidth / 21.0f;
    float cellHeight = gameHeight / 21.0f;

    float screenX = gridX * cellWidth;
    float screenY = gridY * cellHeight;

    sf::FloatRect textBounds = readyText.getLocalBounds();
    readyText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
    readyText.setPosition(screenX, screenY);

    window.draw(readyText);
}

void HUD::drawLevelClearedText(World* world) {
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