#ifndef LEVELSTATE_H
#define LEVELSTATE_H

#include "representation/State.h"
#include "representation/Camera.h"
#include "representation/HUD.h"
#include "logic/World.h"
#include "logic/Score.h"
#include "logic/Lives.h"
#include <memory>
#include <string>

class ConcreteFactory;

class LevelState : public State {
public:
  explicit LevelState(int level, int startingScore = 0);
  ~LevelState() override;

  void onEnter() override;
  void onExit() override;
  void handleEvents(sf::RenderWindow& window) override;
  void update(float deltaTime) override;
  void render(sf::RenderWindow& window) override;

private:
  bool loadLevel();
  void handleInput();

  std::unique_ptr<Lives> lives;
  std::unique_ptr<Score> score;
  std::shared_ptr<Camera> camera;  // ✅ CHANGED from unique_ptr
  std::unique_ptr<HUD> hud;
  std::unique_ptr<World> world;
  std::unique_ptr<ConcreteFactory> factory;

  int currentLevel;
  int initialScore;
  std::string mapFile;
  bool pauseRequested;
};

#endif // LEVELSTATE_H