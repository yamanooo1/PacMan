#ifndef PACMAN_STATE_H
#define PACMAN_STATE_H

#include <SFML/Graphics.hpp>
#include <memory>

// Forward declarations
class StateManager;

class State {
public:
  virtual ~State() = default;

  // Called when state becomes active
  virtual void onEnter() {}

  // Called when state becomes inactive (but stays on stack)
  virtual void onExit() {}

  // ✅ NEW: Called when window is resized
  virtual void onWindowResize(float width, float height) {}

  // Handle SFML events (key presses, mouse clicks, etc.)
  virtual void handleEvents(sf::RenderWindow& window) = 0;

  // Update game logic (called every frame)
  virtual void update(float deltaTime) = 0;

  // Render to screen (called every frame)
  virtual void render(sf::RenderWindow& window) = 0;

  // Should states below this one be visible?
  virtual bool isTransparent() const { return false; }

protected:
  // States need access to StateManager to trigger transitions
  StateManager* stateManager = nullptr;

  friend class StateManager;  // Allow StateManager to set stateManager pointer
};

#endif // PACMAN_STATE_H