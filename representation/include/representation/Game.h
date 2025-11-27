//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <SFML/Graphics.hpp>
#include <memory>

// Forward declarations
class World;
class ConcreteFactory;
class Camera;
class Score;
class Lives;

class Game {
private:
  // Window and rendering
  std::unique_ptr<sf::RenderWindow> window;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<ConcreteFactory> factory;

  // Game logic
  std::unique_ptr<World> world;
  std::unique_ptr<Score> score;
  std::unique_ptr<Lives> lives;

  // Game state
  bool isRunning;

  // Private methods
  void handleEvents();
  void handleInput(float deltaTime);
  void update(float deltaTime);
  void render();

public:
  Game();
  ~Game();

  // Initialize the game (load map, etc.)
  bool initialize(const std::string& mapFile);

  // Main game loop
  void run();
};

#endif //PACMAN_GAME_H