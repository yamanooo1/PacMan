#ifndef PACMAN_PAUSEDSTATE_H
#define PACMAN_PAUSEDSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>

class PausedState : public State {
private:
  sf::Font font;
  sf::Text pausedText;
  sf::Text resumeText;
  sf::Text menuText;

  bool fontLoaded;
  bool resumeRequested;
  bool menuRequested;

public:
  PausedState();
  ~PausedState() override = default;

  void onEnter() override;
  void onExit() override;

  void handleEvents(sf::RenderWindow& window) override;
  void update(float deltaTime) override;
  void render(sf::RenderWindow& window) override;

  bool isTransparent() const override { return true; }

private:
  void setupTexts();
};

#endif // PACMAN_PAUSEDSTATE_H