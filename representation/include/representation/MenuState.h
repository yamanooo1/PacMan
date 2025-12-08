#ifndef PACMAN_MENUSTATE_H
#define PACMAN_MENUSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>

class MenuState : public State {
private:
  sf::Font font;
  sf::Text titleText;

  // ✅ NEW: Play button instead of text
  sf::RectangleShape playButton;
  sf::Text playButtonText;
  bool isPlayButtonHovered;

  // ✅ NEW: How to play section
  sf::Text howToPlayTitle;
  sf::Text howToPlayControls;

  // ✅ NEW: Leaderboard with yellow box
  sf::RectangleShape leaderboardBox;
  sf::Text leaderboardTitle;
  std::vector<sf::Text> scoreNameTexts;  // Player names
  std::vector<sf::Text> scoreValueTexts; // Scores

  bool fontLoaded;

public:
  MenuState();
  ~MenuState() override = default;

  void onEnter() override;
  void onExit() override;

  void handleEvents(sf::RenderWindow& window) override;
  void update(float deltaTime) override;
  void render(sf::RenderWindow& window) override;

private:
  void setupTexts();
  void setupPlayButton();
  void setupHowToPlay();
  void setupLeaderboard();
  bool isMouseOverButton(const sf::RectangleShape& button, sf::RenderWindow& window);
};

#endif // PACMAN_MENUSTATE_H