#ifndef PACMAN_LEVELSTATE_H
#define PACMAN_LEVELSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declarations
class World;
class ConcreteFactory;
class Camera;
class Score;
class Lives;
class HUD;

class LevelState : public State {
private:
  // Rendering
  std::unique_ptr<Camera> camera;
  std::unique_ptr<ConcreteFactory> factory;
  std::unique_ptr<HUD> hud;

  // Game logic
  std::unique_ptr<World> world;
  std::unique_ptr<Score> score;
  std::unique_ptr<Lives> lives;

  // Level management
  int currentLevel;
  std::string mapFile;
  
  // Paused flag (for internal state, ESC key handling)
  bool pauseRequested;

public:
  LevelState(int level = 1);
  ~LevelState() override;

  void onEnter() override;
  void onExit() override;
  
  void handleEvents(sf::RenderWindow& window) override;
  void update(float deltaTime) override;
  void render(sf::RenderWindow& window) override;

private:
  bool loadLevel();
  void handleInput();
};

#endif // PACMAN_LEVELSTATE_H