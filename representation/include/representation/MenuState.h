#ifndef PACMAN_MENUSTATE_H
#define PACMAN_MENUSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>

class MenuState : public State {
private:
  sf::Font font;
  sf::Text titleText;

  // Play button
  sf::RectangleShape playButton;
  sf::Text playButtonText;
  bool isPlayButtonHovered;

  // ✅ NEW: Exit button
  sf::RectangleShape exitButton;
  sf::Text exitButtonText;
  bool isExitButtonHovered;

  // How to play section
  sf::Text howToPlayTitle;
  sf::Text howToPlayControls;
  sf::Text howToPlayPoints;  // ✅ NEW: Points information

  // Leaderboard with yellow box
  sf::RectangleShape leaderboardBox;
  sf::Text leaderboardTitle;
  std::vector<sf::Text> scoreNameTexts;
  std::vector<sf::Text> scoreValueTexts;

  bool fontLoaded;

  // Window size tracking for proportional positioning
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
  void setupExitButton();  // ✅ NEW
  void setupHowToPlay();
  void setupLeaderboard();
  bool isMouseOverButton(const sf::RectangleShape& button, sf::RenderWindow& window);
};

#endif // PACMAN_MENUSTATE_H