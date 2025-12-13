#ifndef PACMAN_STATEMANAGER_H
#define PACMAN_STATEMANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class State;

class StateManager {
public:
    StateManager();
    ~StateManager() = default;

    void pushState(std::unique_ptr<State> state);
    void popState();
    void changeState(std::unique_ptr<State> state);
    void clearStates();
    void clearAndPushState(std::unique_ptr<State> state);

    [[nodiscard]] bool isEmpty() const { return states.empty(); }

    [[nodiscard]] State* getCurrentState() const;

    void handleEvents(sf::RenderWindow& window);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    void onWindowResize(float width, float height);

    void processPendingChanges();

private:
    std::vector<std::unique_ptr<State>> states;

    enum class PendingAction { None, Push, Pop, Change, Clear, ClearAndPush };
    PendingAction pendingAction = PendingAction::None;
    std::unique_ptr<State> pendingState = nullptr;

    float currentWindowWidth = 800.0f;
    float currentWindowHeight = 860.0f;
};

#endif