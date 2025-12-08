#include "representation/StateManager.h"
#include "representation/State.h"
#include <iostream>

StateManager::StateManager() {
}

void StateManager::pushState(std::unique_ptr<State> state) {
  if (!state) {
    std::cerr << "[StateManager] Attempted to push null state!" << std::endl;
    return;
  }

  state->stateManager = this;

  if (!states.empty()) {
    states.back()->onExit();
  }

  states.push_back(std::move(state));
  states.back()->onEnter();

  std::cout << "[StateManager] Pushed new state. Stack size: " << states.size() << std::endl;
}

void StateManager::popState() {
  if (states.empty()) {
    std::cerr << "[StateManager] Attempted to pop from empty state stack!" << std::endl;
    return;
  }

  states.back()->onExit();
  states.pop_back();

  std::cout << "[StateManager] Popped state. Stack size: " << states.size() << std::endl;
}

void StateManager::changeState(std::unique_ptr<State> state) {
  if (!state) {
    std::cerr << "[StateManager] Attempted to change to null state!" << std::endl;
    return;
  }

  if (!states.empty()) {
    states.back()->onExit();
    states.pop_back();
  }

  state->stateManager = this;
  states.push_back(std::move(state));
  states.back()->onEnter();

  std::cout << "[StateManager] Changed state. Stack size: " << states.size() << std::endl;
}

void StateManager::clearStates() {
  std::cout << "[StateManager] Clearing " << states.size() << " states..." << std::endl;

  int stateNum = states.size();
  while (!states.empty()) {
    std::cout << "[StateManager] Exiting state " << stateNum << "..." << std::endl;
    states.back()->onExit();
    std::cout << "[StateManager] Popping state " << stateNum << "..." << std::endl;
    states.pop_back();
    stateNum--;
    std::cout << "[StateManager] State " << (stateNum + 1) << " destroyed. Remaining: " << states.size() << std::endl;
  }

  std::cout << "[StateManager] All states cleared." << std::endl;
}

void StateManager::clearAndPushState(std::unique_ptr<State> state) {
  if (!state) {
    std::cerr << "[StateManager] Attempted to clear and push null state!" << std::endl;
    return;
  }

  std::cout << "[StateManager] Requesting clear and push (deferred)" << std::endl;
  pendingAction = PendingAction::ClearAndPush;
  pendingState = std::move(state);
}

State* StateManager::getCurrentState() const {
  if (states.empty()) {
    return nullptr;
  }
  return states.back().get();
}

void StateManager::handleEvents(sf::RenderWindow& window) {
  if (!states.empty()) {
    states.back()->handleEvents(window);
  }
}

void StateManager::update(float deltaTime) {
  if (!states.empty()) {
    states.back()->update(deltaTime);
  }
}

void StateManager::render(sf::RenderWindow& window) {
  if (states.empty()) return;

  int startIndex = states.size() - 1;

  for (int i = states.size() - 2; i >= 0; --i) {
    if (!states[i + 1]->isTransparent()) {
      startIndex = i + 1;
      break;
    }
    startIndex = i;
  }

  for (size_t i = startIndex; i < states.size(); ++i) {
    states[i]->render(window);
  }
}

// ✅ NEW: Propagate window resize to current state
void StateManager::onWindowResize(float width, float height) {
  if (!states.empty()) {
    states.back()->onWindowResize(width, height);
    std::cout << "[StateManager] Propagated resize (" << width << "x" << height
              << ") to current state" << std::endl;
  }
}

void StateManager::processPendingChanges() {
  if (pendingAction == PendingAction::None) {
    return;
  }

  std::cout << "[StateManager] Processing pending changes..." << std::endl;

  switch (pendingAction) {
  case PendingAction::ClearAndPush:
    std::cout << "[StateManager] Clearing states..." << std::endl;
    clearStates();
    std::cout << "[StateManager] States cleared, now pushing new state..." << std::endl;
    pushState(std::move(pendingState));
    std::cout << "[StateManager] New state pushed." << std::endl;
    break;

  default:
    break;
  }

  pendingAction = PendingAction::None;
  pendingState = nullptr;
  std::cout << "[StateManager] Pending changes processed." << std::endl;
}