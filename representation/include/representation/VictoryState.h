#ifndef PACMAN_VICTORYSTATE_H
#define PACMAN_VICTORYSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>

class VictoryState : public State {
private:
  sf::Font font;
  sf::Text titleText;
  sf::Text scoreText;
  sf::Text continueText;
  sf::Text menuText;

  bool fontLoaded;
  bool continueRequested;
  bool menuRequested;

  int nextLevel;     // 0 = game over (no next level)
  int finalScore;
  bool isGameOver;   // true if game over, false if level cleared

public:
  VictoryState(int nextLevelNum, int score);
  ~VictoryState() override = default;

  void onEnter() override;
  void onExit() override;

  void handleEvents(sf::RenderWindow& window) override;
  void update(float deltaTime) override;
  void render(sf::RenderWindow& window) override;

  bool isTransparent() const override { return true; }

private:
  void setupTexts();
};

#endif // PACMAN_VICTORYSTATE_H