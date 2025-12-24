#ifndef LEVELSTATE_H
#define LEVELSTATE_H

#include "../../../../logic/include/logic/core/World.h"
#include "../../../../logic/include/logic/systems/Lives.h"
#include "../../../../logic/include/logic/systems/Score.h"
#include "../core/Camera.h"
#include "../systems/HUD.h"
#include "State.h"
#include <memory>
#include <string>

namespace representation {

class ConcreteFactory;

class LevelState : public State {
public:
    explicit LevelState(int level, int startingScore = 0);
    ~LevelState() override;

    void onEnter() override;
    void onExit() override;
    void onWindowResize(float width, float height) override;
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    bool loadLevel();
    void handleInput();

    std::shared_ptr<logic::Lives> lives;
    std::shared_ptr<logic::Score> score;
    std::shared_ptr<Camera> camera;
    std::unique_ptr<HUD> hud;
    std::unique_ptr<logic::World> world;
    std::unique_ptr<ConcreteFactory> factory;

    int currentLevel;
    int initialScore;
    std::string mapFile;
    bool pauseRequested;

    float windowWidth;
    float windowHeight;
    static constexpr float HUD_HEIGHT = 60.0f;
};

} // namespace representation

#endif