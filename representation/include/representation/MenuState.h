#ifndef PACMAN_MENUSTATE_H
#define PACMAN_MENUSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>

class MenuState : public State {
private:
  sf::Font font;
  sf::Text titleText;
  sf::Text playText;
  sf::Text instructionsText;
  sf::Text leaderboardTitle;

  // ✅ NEW: Store high scores and score texts
  std::vector<int> highScores;
  std::vector<sf::Text> scoreTexts;

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
};

#endif // PACMAN_MENUSTATE_H