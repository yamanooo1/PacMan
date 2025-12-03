#include "representation/HUD.h"
#include "logic/World.h"
#include "logic/Score.h"
#include "logic/Lives.h"
#include <iostream>

HUD::HUD(sf::RenderWindow* window, int hudHeight)
    : window(window)
    , hudHeight(hudHeight)
    , fontLoaded(false)
{
}

void HUD::loadFont(const std::string& fontPath) {
    if (font.loadFromFile(fontPath)) {
        fontLoaded = true;
        std::cout << "[HUD] Font loaded: " << fontPath << std::endl;
    } else {
        std::cerr << "[HUD] Failed to load font: " << fontPath << std::endl;
        fontLoaded = false;
    }
}

void HUD::draw(World* world, Score* score, Lives* lives) {
    if (!window || !score || !lives) return;

    // Calculate HUD position dynamically based on current window size
    float windowHeight = static_cast<float>(window->getSize().y);
    float hudYPosition = windowHeight - hudHeight;

    // Draw black background for HUD area
    sf::RectangleShape hudBackground;
    hudBackground.setSize(sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(hudHeight)));
    hudBackground.setPosition(0.f, hudYPosition);
    hudBackground.setFillColor(sf::Color::Black);
    window->draw(hudBackground);

    if (!fontLoaded) {
        return;
    }

    // Get current score and lives
    int currentScore = score->getScore();
    int currentLives = lives->getLives();

    // Draw score text
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("SCORE: " + std::to_string(currentScore));
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, hudYPosition + 20.f);
    window->draw(scoreText);

    // Draw lives text
    sf::Text livesText;
    livesText.setFont(font);
    livesText.setString("LIVES: " + std::to_string(currentLives));
    livesText.setCharacterSize(20);
    livesText.setFillColor(sf::Color::White);

    // Position on right side
    sf::FloatRect textBounds = livesText.getLocalBounds();
    livesText.setPosition(window->getSize().x - textBounds.width - 20.f, hudYPosition + 20.f);
    window->draw(livesText);
}