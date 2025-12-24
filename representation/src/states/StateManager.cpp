#include "../../include/representation/states/StateManager.h"
#include "../../include/representation/states/State.h"

namespace representation {

StateManager::StateManager() {}

/**
 * @brief Push new state with full lifecycle
 *
 * State activation sequence ensures clean transitions.
 */
void StateManager::pushState(std::unique_ptr<State> state) {
    if (!state) {
        return;
    }

    state->stateManager = this;

    // Deactivate current state
    if (!states.empty()) {
        states.back()->onExit();
    }

    states.push_back(std::move(state));

    // Notify of current window size
    states.back()->onWindowResize(currentWindowWidth, currentWindowHeight);

    // Activate new state
    states.back()->onEnter();
}

/**
 * @brief Remove and cleanup current state
 */
void StateManager::popState() {
    if (states.empty()) {
        return;
    }

    states.back()->onExit();
    states.pop_back();
}

/**
 * @brief Replace current state atomically
 */
void StateManager::changeState(std::unique_ptr<State> state) {
    if (!state) {
        return;
    }

    // Remove current state
    if (!states.empty()) {
        states.back()->onExit();
        states.pop_back();
    }

    // Push new state
    state->stateManager = this;
    states.push_back(std::move(state));

    states.back()->onWindowResize(currentWindowWidth, currentWindowHeight);

    states.back()->onEnter();
}

/**
 * @brief Clear entire state stack
 *
 * Pops in reverse order for clean shutdown.
 */
void StateManager::clearStates() {
    int stateNum = states.size();
    while (!states.empty()) {
        states.back()->onExit();
        states.pop_back();
        stateNum--;
    }
}

/**
 * @brief Defer clear+push operation
 *
 * Cannot execute immediately (might be called during update).
 * Stored in pending system.
 */
void StateManager::clearAndPushState(std::unique_ptr<State> state) {
    if (!state) {
        return;
    }

    pendingAction = PendingAction::ClearAndPush;
    pendingState = std::move(state);
}

State* StateManager::getCurrentState() const {
    if (states.empty()) {
        return nullptr;
    }
    return states.back().get();
}

/**
 * @brief Forward events to active state only
 */
void StateManager::handleEvents(sf::RenderWindow& window) {
    if (!states.empty()) {
        states.back()->handleEvents(window);
    }
}

/**
 * @brief Update active state only
 */
void StateManager::update(float deltaTime) {
    if (!states.empty()) {
        states.back()->update(deltaTime);
    }
}

/**
 * @brief Render states respecting transparency
 *
 * Finds rendering start point by checking transparency.
 * Renders all visible states bottom-to-top.
 */
void StateManager::render(sf::RenderWindow& window) {
    if (states.empty())
        return;

    // Find first opaque state from top
    int startIndex = states.size() - 1;

    for (int i = states.size() - 2; i >= 0; --i) {
        if (!states[i + 1]->isTransparent()) {
            startIndex = i + 1;
            break;
        }
        startIndex = i;
    }

    // Render from startIndex to top
    for (size_t i = startIndex; i < states.size(); ++i) {
        states[i]->render(window);
    }
}

/**
 * @brief Store window size and notify active state
 */
void StateManager::onWindowResize(float width, float height) {
    currentWindowWidth = width;
    currentWindowHeight = height;

    // Only notify active state
    if (!states.empty()) {
        states.back()->onWindowResize(width, height);
    }
}

/**
 * @brief Execute deferred state transitions
 *
 * Called at end of frame after rendering.
 * Ensures safe state transitions.
 */
void StateManager::processPendingChanges() {
    if (pendingAction == PendingAction::None) {
        return;
    }

    switch (pendingAction) {
    case PendingAction::ClearAndPush:
        clearStates();
        pushState(std::move(pendingState));
        break;

    default:
        break;
    }

    pendingAction = PendingAction::None;
    pendingState = nullptr;
}

} // namespace representation