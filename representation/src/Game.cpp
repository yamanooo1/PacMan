#include "representation/Game.h"
#include "representation/StateManager.h"
#include "representation/MenuState.h"
#include "logic/Stopwatch.h"
#include <iostream>

Game::Game()
    : window(nullptr)
    , stateManager(nullptr)
    , isRunning(false)
{
}

Game::~Game() {
}

bool Game::initialize() {
  std::cout << "[GAME] Initializing game..." << std::endl;

  // Create window
  window = std::make_unique<sf::RenderWindow>(
      sf::VideoMode(800, 860),  // 800x800 game + 60px HUD
      "PacMan"
  );

  // Create state manager
  stateManager = std::make_unique<StateManager>();

  // Push initial MenuState
  stateManager->pushState(std::make_unique<MenuState>());

  std::cout << "[GAME] Initialization complete!" << std::endl;

  isRunning = true;
  return true;
}

void Game::run() {
  if (!isRunning) {
    std::cerr << "[GAME] Game not initialized! Call initialize() first." << std::endl;
    return;
  }

  Stopwatch& stopwatch = Stopwatch::getInstance();

  while (window->isOpen() && isRunning) {
    stopwatch.tick();
    float deltaTime = stopwatch.getDeltaTime();

    handleEvents();

    // Check if we have states
    if (stateManager->isEmpty()) {
      std::cout << "[GAME] No more states - exiting..." << std::endl;
      isRunning = false;
      window->close();
      break;
    }

    // Update current state
    stateManager->update(deltaTime);

    // Render
    window->clear(sf::Color::Black);
    stateManager->render(*window);
    window->display();

    // Process deferred state changes
    stateManager->processPendingChanges();

    // Check again after processing changes
    if (stateManager->isEmpty()) {
      std::cout << "[GAME] No states after processing changes - exiting..." << std::endl;
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
      // Update window view to maintain aspect ratio
      float windowWidth = static_cast<float>(event.size.width);
      float windowHeight = static_cast<float>(event.size.height);

      sf::FloatRect visibleArea(0, 0, windowWidth, windowHeight);
      window->setView(sf::View(visibleArea));

      // ✅ NEW: Notify state manager of resize so it can update cameras
      if (stateManager) {
        stateManager->onWindowResize(windowWidth, windowHeight);
      }

      std::cout << "[GAME] Window resized to " << windowWidth << "x" << windowHeight << std::endl;
    }

    // Pass event to current state
    stateManager->handleEvents(*window);
  }
}