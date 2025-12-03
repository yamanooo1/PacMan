#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <SFML/Graphics.hpp>
#include <memory>

// Forward declarations
class StateManager;

class Game {
private:
  // Window
  std::unique_ptr<sf::RenderWindow> window;

  // State management
  std::unique_ptr<StateManager> stateManager;

  // Game state
  bool isRunning;

  // Private methods
  void handleEvents();

public:
  Game();
  ~Game();

  // Initialize the game (create window, setup initial state)
  bool initialize();

  // Main game loop
  void run();

  // Get window (needed by states)
  sf::RenderWindow* getWindow() const { return window.get(); }
};

#endif // PACMAN_GAME_H