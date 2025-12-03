#ifndef PACMAN_STATEMANAGER_H
#define PACMAN_STATEMANAGER_H

#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

class State;

class StateManager {
public:
  StateManager();
  ~StateManager() = default;

  // Push a new state onto the stack
  void pushState(std::unique_ptr<State> state);

  // Pop the current state from the stack
  void popState();

  // Replace current state with a new one (pop then push)
  void changeState(std::unique_ptr<State> state);

  // Clear all states
  void clearStates();

  // ✅ NEW: Clear all and push a new state (deferred)
  void clearAndPushState(std::unique_ptr<State> state);

  // Check if we have any states
  bool isEmpty() const { return states.empty(); }

  // Get current active state (top of stack)
  State* getCurrentState() const;

  // Main loop functions - delegate to current state
  void handleEvents(sf::RenderWindow& window);
  void update(float deltaTime);
  void render(sf::RenderWindow& window);

  // ✅ NEW: Process pending changes (call at end of frame)
  void processPendingChanges();

private:
  // State stack
  std::vector<std::unique_ptr<State>> states;

  // ✅ NEW: Pending operations
  enum class PendingAction { None, Push, Pop, Change, Clear, ClearAndPush };
  PendingAction pendingAction = PendingAction::None;
  std::unique_ptr<State> pendingState = nullptr;
};

#endif // PACMAN_STATEMANAGER_H