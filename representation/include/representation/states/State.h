#ifndef PACMAN_STATE_H
#define PACMAN_STATE_H

#include <SFML/Graphics.hpp>

class StateManager;

class State {
public:
  virtual ~State() = default;

  virtual void onEnter() {}
  virtual void onExit() {}
  virtual void onWindowResize(float width, float height) {}

  virtual void handleEvents(sf::RenderWindow& window) = 0;
  virtual void update(float deltaTime) = 0;
  virtual void render(sf::RenderWindow& window) = 0;

  virtual bool isTransparent() const { return false; }

protected:
  StateManager* stateManager = nullptr;

  friend class StateManager;
};

#endif