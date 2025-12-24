#ifndef PACMAN_VICTORYSTATE_H
#define PACMAN_VICTORYSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace representation {

class VictoryState : public State {
private:
    sf::Font font;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text continueText;
    sf::Text menuText;

    sf::Text namePromptText;
    sf::Text nameInputText;
    sf::RectangleShape nameInputBox;
    std::string playerNameInput;
    bool isEnteringName;
    bool nameSubmitted;

    bool fontLoaded;
    bool continueRequested;
    bool menuRequested;

    bool spaceWasPressed;
    bool mWasPressed;

    int nextLevel;
    int finalScore;
    bool isGameOver;
    bool qualifiesForHighScore;

    float windowWidth;
    float windowHeight;

public:
    VictoryState(int nextLevelNum, int score);
    ~VictoryState() override = default;

    void onEnter() override;
    void onExit() override;
    void onWindowResize(float width, float height) override;

    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    bool isTransparent() const override { return true; }

private:
    void setupTexts();
    void setupNameInput();
    void handleTextInput(sf::Uint32 unicode);
    void submitHighScore();
};

} // namespace representation

#endif