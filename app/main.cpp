/**
 * @file main.cpp
 * @brief PacMan Application Entry Point
 *
 * ARCHITECTURE: Application launcher
 *
 * Purpose:
 * - Create and initialize Game instance
 * - Run main game loop
 * - Handle initialization failures
 *
 * Execution Flow:
 * ```
 * main()
 *   → Game::Game()            (construct)
 *   → Game::initialize()      (setup window, state manager, menu)
 *     ├─ Create SFML window (800×860)
 *     ├─ Create StateManager
 *     ├─ Notify window size
 *     └─ Push MenuState (starting screen)
 *   → Game::run()             (game loop)
 *     └─ while (window.isOpen())
 *          ├─ stopwatch.tick()          (update delta time)
 *          ├─ handleEvents()            (input, window events)
 *          ├─ stateManager->update(dt)  (game logic)
 *          ├─ window.clear()            (clear frame)
 *          ├─ stateManager->render()    (draw)
 *          ├─ window.display()          (present)
 *          └─ processPendingChanges()   (state transitions)
 *   → Game::~Game()           (cleanup)
 * ```
 *
 * Error Handling:
 * - initialize() failure: Print error, return 1
 * - Success: Return 0
 *
 * State Machine Bootstrap:
 * - Game creates StateManager
 * - StateManager starts with MenuState
 * - User navigates: Menu → Level → Pause/Victory → ...
 * - Empty state stack → exit application
 *
 * Window Configuration:
 * - Size: 800×860 pixels (800 game + 60 HUD)
 * - Title: "PacMan"
 * - Resizable: Yes (states handle resize events)
 * - V-Sync: Default (no explicit control)
 *
 * Dependencies:
 * - representation::Game (main game class)
 * - Logic library (via Game → StateManager → LevelState → World)
 * - SFML (via Game → sf::RenderWindow)
 *
 * Build System:
 * - CMakeLists.txt links: representation + logic
 * - Representation links: SFML (graphics, window, system, audio)
 * - Logic: Header-only to representation
 *
 * @see representation::Game for game management
 * @see representation::StateManager for state control
 * @see representation::MenuState for starting state
 */

#include "../representation/include/representation/core/Game.h"
#include <iostream>

int main() {
    // Create game instance
    representation::Game game;

    // Initialize game systems
    if (!game.initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return 1;
    }

    // Run main game loop (blocks until exit)
    game.run();

    return 0;
}