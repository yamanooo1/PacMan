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
    explicit LevelState(int level, int startingScore = 0);  // ✅ Added startingScore parameter with default value
    ~LevelState() override;

    void onEnter() override;
    void onExit() override;
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    bool loadLevel();
    void handleInput();

  std::unique_ptr<Lives> lives;            // Destroyed LAST (6th)
  std::unique_ptr<Score> score;            // Destroyed 5th
  std::unique_ptr<Camera> camera;          // Destroyed 4th
  std::unique_ptr<HUD> hud;                // Destroyed 3rd
  std::unique_ptr<World> world;            // Destroyed 2nd (models destroyed here)
  std::unique_ptr<ConcreteFactory> factory; // Destroyed FIRST ✅ (Views detach from models)

  int currentLevel;
  int initialScore;
  std::string mapFile;
  bool pauseRequested;
};

#endif // LEVELSTATE_H