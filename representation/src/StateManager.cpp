#include "representation/StateManager.h"
#include "representation/State.h"

StateManager::StateManager() {
}

void StateManager::pushState(std::unique_ptr<State> state) {
  if (!state) {
    return;
  }

  state->stateManager = this;

  if (!states.empty()) {
    states.back()->onExit();
  }

  states.push_back(std::move(state));

  states.back()->onWindowResize(currentWindowWidth, currentWindowHeight);

  states.back()->onEnter();
}

void StateManager::popState() {
  if (states.empty()) {
    return;
  }

  states.back()->onExit();
  states.pop_back();
}

void StateManager::changeState(std::unique_ptr<State> state) {
  if (!state) {
    return;
  }

  if (!states.empty()) {
    states.back()->onExit();
    states.pop_back();
  }

  state->stateManager = this;
  states.push_back(std::move(state));

  states.back()->onWindowResize(currentWindowWidth, currentWindowHeight);

  states.back()->onEnter();
}

void StateManager::clearStates() {
  int stateNum = states.size();
  while (!states.empty()) {
    states.back()->onExit();
    states.pop_back();
    stateNum--;
  }
}

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

void StateManager::onWindowResize(float width, float height) {
  currentWindowWidth = width;
  currentWindowHeight = height;

  if (!states.empty()) {
    states.back()->onWindowResize(width, height);
  }
}

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