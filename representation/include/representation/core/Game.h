#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

class StateManager;

/**
 * @brief Game - main application class managing window and game loop
 *
 * Purpose:
 * - Initialize SFML window
 * - Create state manager (handles menu/level/pause states)
 * - Run main game loop (event handling, update, render)
 * - Handle window events (close, resize)
 *
 * Architecture:
 * ```
 * main()
 *   → Game game
 *   → game.initialize() ────→ Creates window (800×860)
 *   → game.run()            → Creates StateManager
 *                           → Pushes MenuState
 *
 *   Game Loop:
 *   while (window.isOpen()) {
 *     handleEvents()  ────→ Process SF events
 *     update()        ────→ StateManager::update(deltaTime)
 *     render()        ────→ StateManager::render(window)
 *   }
 * ```
 *
 * Window Configuration:
 * - Size: 800×860 pixels (800 for game, 60 for HUD)
 * - Title: "PacMan"
 * - Resizable: Yes (states handle resize)
 *
 * State Management:
 * - StateManager owns state stack (menu, level, pause, victory)
 * - Game delegates all gameplay logic to StateManager
 * - Window events forwarded to active state
 *
 * Game Loop Timing:
 * - No explicit FPS cap (SFML uses VSync by default)
 * - Stopwatch singleton tracks deltaTime for smooth movement
 * - Updates driven by frame time, not fixed timestep
 *
 * Event Handling Priority:
 * 1. Window close → terminate
 * 2. Window resize → update view, notify StateManager
 * 3. Other events → forwarded to StateManager
 *
 * Shutdown Conditions:
 * - Window closed by user
 * - StateManager empty (all states popped)
 * - isRunning set to false
 *
 * @see StateManager for state machine implementation
 * @see State for state interface
 */
class Game {
private:
    std::unique_ptr<sf::RenderWindow> window;   // SFML rendering window
    std::unique_ptr<StateManager> stateManager; // State machine controller
    bool isRunning;                             // Game loop control flag

    /**
     * @brief Handle SFML window events
     *
     * Processes:
     * - Window close → stop game loop
     * - Window resize → adjust view, notify StateManager
     * - Input events → forward to StateManager
     *
     * Called once per frame at start of game loop.
     */
    void handleEvents();

public:
    Game();
    ~Game();

    /**
     * @brief Initialize game systems
     *
     * Creates:
     * - SFML window (800×860)
     * - StateManager
     * - Initial MenuState
     *
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Run main game loop
     *
     * Loop structure:
     * ```
     * while (window.isOpen() && isRunning) {
     *   stopwatch.tick()           // Update deltaTime
     *   handleEvents()             // Process window events
     *   stateManager->update(dt)   // Update game state
     *   window.clear()             // Clear frame
     *   stateManager->render()     // Draw current state
     *   window.display()           // Present frame
     *   processPendingChanges()    // Apply state transitions
     * }
     * ```
     *
     * Exits when:
     * - Window closed
     * - StateManager empty (game ended)
     */
    void run();

    /**
     * @brief Get window pointer (used by states for rendering)
     *
     * @return Raw pointer to SFML window
     */
    [[nodiscard]] sf::RenderWindow* getWindow() const { return window.get(); }
};

} // namespace representation

#endif