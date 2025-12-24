#ifndef PACMAN_STATE_H
#define PACMAN_STATE_H

#include <SFML/Graphics.hpp>

namespace representation {

class StateManager;

/**
 * @brief State - abstract base class for game states
 *
 * DESIGN PATTERN: State Pattern (manages game screens/modes)
 *
 * Purpose:
 * - Defines interface for game states (menu, level, pause, victory)
 * - Each state handles its own input, update, and rendering
 * - StateManager controls state stack and transitions
 *
 * State Lifecycle:
 * ```
 * State created
 *   → onEnter()     (initialization: load resources, play music)
 *   → Loop:
 *       handleEvents(window)  (process input)
 *       update(deltaTime)     (game logic)
 *       render(window)        (draw to screen)
 *   → onExit()      (cleanup: stop music, save data)
 * State destroyed
 * ```
 *
 * State Types:
 * 1. **MenuState**: Main menu (play, exit, leaderboard)
 * 2. **LevelState**: Active gameplay (PacMan, ghosts, maze)
 * 3. **PausedState**: Paused overlay (transparent, resume/menu)
 * 4. **VictoryState**: Level complete/game over (score, continue)
 *
 * Transparency System:
 * - Opaque states (false): Render only this state
 * - Transparent states (true): Render previous state + this state
 * - Example: PausedState is transparent (shows game behind)
 *
 * State Transitions:
 * - Push: Add new state on top (pause game → push PausedState)
 * - Pop: Remove current state (resume → pop PausedState)
 * - Change: Replace current state (menu → level)
 * - Clear: Remove all states (exit game)
 *
 * StateManager Communication:
 * - State holds pointer to StateManager (set by StateManager)
 * - State can request transitions via stateManager->pushState()
 * - Transitions deferred until end of frame (processPendingChanges)
 *
 * Window Resize Handling:
 * - onWindowResize() called when window size changes
 * - State repositions UI elements, updates camera
 * - Maintains proper layout at any resolution
 *
 * @see StateManager for state stack management
 */
class State {
public:
    virtual ~State() = default;

    /**
     * @brief Called when state becomes active (pushed to stack)
     *
     * Use for:
     * - Loading resources
     * - Starting music
     * - Initializing game state
     * - Resetting timers
     *
     * Example (LevelState):
     * - Load sprite sheet
     * - Create world
     * - Start background music
     */
    virtual void onEnter() {}

    /**
     * @brief Called when state becomes inactive (popped from stack)
     *
     * Use for:
     * - Stopping music
     * - Saving data
     * - Cleanup
     *
     * Example (LevelState):
     * - Stop all sounds
     * - Save high scores
     */
    virtual void onExit() {}

    /**
     * @brief Called when window resizes
     *
     * Use for:
     * - Repositioning UI elements
     * - Updating camera viewport
     * - Recalculating button bounds
     *
     * @param width New window width in pixels
     * @param height New window height in pixels
     */
    virtual void onWindowResize(float width, float height) {}

    /**
     * @brief Handle SFML input events (pure virtual)
     *
     * Process:
     * - Keyboard input (WASD, arrows, ESC, SPACE)
     * - Mouse clicks (menu buttons)
     * - Window events (already handled by Game)
     *
     * Example (MenuState):
     * - Check if mouse over Play button → start game
     * - Check if ESC pressed → exit application
     *
     * @param window SFML window for input queries
     */
    virtual void handleEvents(sf::RenderWindow& window) = 0;

    /**
     * @brief Update state logic (pure virtual)
     *
     * Called once per frame with delta time.
     *
     * Example (LevelState):
     * - Update world (entities, collisions)
     * - Update score decay
     * - Check win/lose conditions
     *
     * @param deltaTime Time since last frame in seconds
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render state to window (pure virtual)
     *
     * Called once per frame after update.
     *
     * Example (LevelState):
     * - Draw all entities
     * - Draw HUD (score, lives)
     * - Draw special overlays (READY!, GAME OVER)
     *
     * @param window SFML window to draw to
     */
    virtual void render(sf::RenderWindow& window) = 0;

    /**
     * @brief Check if state is transparent
     *
     * Transparent states render over previous state.
     * Used for overlays like pause menu.
     *
     * Default: false (opaque, render only this state)
     * Override: return true for overlays
     *
     * @return true if previous state should also render
     */
    virtual bool isTransparent() const { return false; }

protected:
    /**
     * @brief Pointer to StateManager (set by StateManager::pushState)
     *
     * Used to request state transitions:
     * - stateManager->pushState() - Add overlay
     * - stateManager->popState() - Remove current
     * - stateManager->changeState() - Replace current
     * - stateManager->clearAndPushState() - Reset to new state
     *
     * Never null when state is active.
     */
    StateManager* stateManager = nullptr;

    friend class StateManager;
};

} // namespace representation

#endif