#include "representation/Game.h"
#include "representation/StateManager.h"
#include "representation/MenuState.h"
#include "logic/Stopwatch.h"

Game::Game()
    : window(nullptr)
    , stateManager(nullptr)
    , isRunning(false) {
}

Game::~Game() {
}

bool Game::initialize() {
  window = std::make_unique<sf::RenderWindow>(
      sf::VideoMode(800, 860),
      "PacMan"
  );

  stateManager = std::make_unique<StateManager>();

  stateManager->onWindowResize(window->getSize().x, window->getSize().y);

  stateManager->pushState(std::make_unique<MenuState>());

  isRunning = true;
  return true;
}

void Game::run() {
  if (!isRunning) {
    return;
  }

  Stopwatch& stopwatch = Stopwatch::getInstance();

  while (window->isOpen() && isRunning) {
    stopwatch.tick();
    float deltaTime = stopwatch.getDeltaTime();

    handleEvents();

    if (stateManager->isEmpty()) {
      isRunning = false;
      window->close();
      break;
    }

    stateManager->update(deltaTime);

    window->clear(sf::Color::Black);
    stateManager->render(*window);
    window->display();

    stateManager->processPendingChanges();

    if (stateManager->isEmpty()) {
      isRunning = false;
      window->close();
      break;
    }
  }
}

void Game::handleEvents() {
  sf::Event event;
  while (window->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window->close();
      isRunning = false;
    }

    if (event.type == sf::Event::Resized) {
      float windowWidth = static_cast<float>(event.size.width);
      float windowHeight = static_cast<float>(event.size.height);

      sf::FloatRect visibleArea(0, 0, windowWidth, windowHeight);
      window->setView(sf::View(visibleArea));

      if (stateManager) {
        stateManager->onWindowResize(windowWidth, windowHeight);
      }
    }

    stateManager->handleEvents(*window);
  }
}