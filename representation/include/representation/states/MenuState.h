#ifndef PACMAN_MENUSTATE_H
#define PACMAN_MENUSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>

class MenuState : public State {
private:
  sf::Font font;
  sf::Text titleText;

  sf::RectangleShape playButton;
  sf::Text playButtonText;
  bool isPlayButtonHovered;

  sf::RectangleShape exitButton;
  sf::Text exitButtonText;
  bool isExitButtonHovered;

  sf::Text howToPlayTitle;
  sf::Text howToPlayControls;
  sf::Text howToPlayPoints;

  sf::RectangleShape leaderboardBox;
  sf::Text leaderboardTitle;
  std::vector<sf::Text> scoreNameTexts;
  std::vector<sf::Text> scoreValueTexts;

  bool fontLoaded;

  float windowWidth;
  float windowHeight;

public:
  MenuState();
  ~MenuState() override = default;

  void onEnter() override;
  void onWindowResize(float width, float height) override;

  void handleEvents(sf::RenderWindow& window) override;
  void update(float deltaTime) override;
  void render(sf::RenderWindow& window) override;

private:
  void setupTexts();
  void setupPlayButton();
  void setupExitButton();
  void setupHowToPlay();
  void setupLeaderboard();
  bool isMouseOverButton(const sf::RectangleShape& button, sf::RenderWindow& window);
};

#endif