#include "../../include/representation/core/Game.h"
#include "../../../logic/include/logic/utils/Stopwatch.h"
#include "../../include/representation/states/MenuState.h"
#include "../../include/representation/states/StateManager.h"

Game::Game() : window(nullptr), stateManager(nullptr), isRunning(false) {}

Game::~Game() = default;

bool Game::initialize() {
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 860), "PacMan");

    stateManager = std::make_unique<StateManager>();

    stateManager->onWindowResize(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y));

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
            auto windowWidth = static_cast<float>(event.size.width);
            auto windowHeight = static_cast<float>(event.size.height);

            sf::FloatRect visibleArea(0, 0, windowWidth, windowHeight);
            window->setView(sf::View(visibleArea));

            if (stateManager) {
                stateManager->onWindowResize(windowWidth, windowHeight);
            }
        }

        stateManager->handleEvents(*window);
    }
}