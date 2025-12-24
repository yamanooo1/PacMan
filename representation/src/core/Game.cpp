#include "../../include/representation/core/Game.h"
#include "../../../logic/include/logic/utils/Stopwatch.h"
#include "../../include/representation/states/MenuState.h"
#include "../../include/representation/states/StateManager.h"

namespace representation {

/**
 * @brief Initialize game with null state
 *
 * Window and StateManager created in initialize().
 */
Game::Game() : window(nullptr), stateManager(nullptr), isRunning(false) {}

Game::~Game() = default;

/**
 * @brief Initialize game systems
 *
 * Setup sequence:
 * 1. Create SFML window (800×860)
 * 2. Create StateManager
 * 3. Notify initial window size
 * 4. Push MenuState (starting screen)
 * 5. Set isRunning flag
 *
 * Window Size Rationale:
 * - Width: 800px for 21×21 grid (38px per cell)
 * - Height: 860px (800 game + 60 HUD)
 */
bool Game::initialize() {
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 860), "PacMan");

    stateManager = std::make_unique<StateManager>();

    stateManager->onWindowResize(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y));

    stateManager->pushState(std::make_unique<MenuState>());

    isRunning = true;
    return true;
}

/**
 * @brief Main game loop
 *
 * Loop Phases:
 * 1. Time: Update Stopwatch deltaTime
 * 2. Input: Process window events
 * 3. Update: Update active state logic
 * 4. Render: Clear, draw state, display
 * 5. Cleanup: Apply pending state changes
 *
 * Exit Conditions:
 * - Window closed
 * - StateManager empty (twice checked for safety)
 * - isRunning false
 */
void Game::run() {
    if (!isRunning) {
        return;
    }

    logic::Stopwatch& stopwatch = logic::Stopwatch::getInstance();

    while (window->isOpen() && isRunning) {
        stopwatch.tick();
        float deltaTime = stopwatch.getDeltaTime();

        handleEvents();

        // Check if states emptied (game ended)
        if (stateManager->isEmpty()) {
            isRunning = false;
            window->close();
            break;
        }

        stateManager->update(deltaTime);

        window->clear(sf::Color::Black);
        stateManager->render(*window);
        window->display();

        // Apply state transitions after rendering
        stateManager->processPendingChanges();

        // Double-check state emptying
        if (stateManager->isEmpty()) {
            isRunning = false;
            window->close();
            break;
        }
    }
}

/**
 * @brief Process SFML events
 *
 * Event Handling:
 * 1. Close event → shut down window + game loop
 * 2. Resize event → update SFML view, notify StateManager
 * 3. Other events → forwarded to StateManager
 *
 * Resize Handling:
 * - Create new view with updated dimensions
 * - Prevents stretching/distortion
 * - StateManager updates Camera, repositions UI elements
 */
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

            // Update SFML view to prevent distortion
            sf::FloatRect visibleArea(0, 0, windowWidth, windowHeight);
            window->setView(sf::View(visibleArea));

            if (stateManager) {
                stateManager->onWindowResize(windowWidth, windowHeight);
            }
        }

        stateManager->handleEvents(*window);
    }
}

} // namespace representation