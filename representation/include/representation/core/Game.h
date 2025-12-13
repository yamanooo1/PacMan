#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <SFML/Graphics.hpp>
#include <memory>

class StateManager;

class Game {
private:
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<StateManager> stateManager;
    bool isRunning;

    void handleEvents();

public:
    Game();
    ~Game();

    bool initialize();
    void run();

    [[nodiscard]] sf::RenderWindow* getWindow() const { return window.get(); }
};

#endif