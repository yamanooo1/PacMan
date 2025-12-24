#ifndef PACMAN_STATEMANAGER_H
#define PACMAN_STATEMANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace representation {

class State;

/**
 * @brief StateManager - manages state stack and transitions
 *
 * DESIGN PATTERN: State Pattern + Stack-based State Machine
 *
 * Purpose:
 * - Maintain stack of active states
 * - Handle state transitions (push, pop, change)
 * - Coordinate rendering (transparent state support)
 * - Defer transitions to avoid mid-frame corruption
 *
 * State Stack Architecture:
 * ```
 * Stack (top = active):
 * ┌─────────────────┐
 * │ VictoryState    │ ← Active (handles input/update)
 * ├─────────────────┤
 * │ LevelState      │ ← Rendered if VictoryState transparent
 * ├─────────────────┤
 * │ MenuState       │ ← Inactive
 * └─────────────────┘
 * ```
 *
 * State Transition Examples:
 *
 * 1. **Start Game**:
 *    - Menu: pushState(LevelState(1))
 *    - Stack: [Menu, Level]
 *    - Effect: Menu→onExit, Level→onEnter
 *
 * 2. **Pause**:
 *    - Level: pushState(PausedState())
 *    - Stack: [Menu, Level, Paused]
 *    - Rendering: Level + Paused (transparent)
 *
 * 3. **Resume**:
 *    - Paused: popState()
 *    - Stack: [Menu, Level]
 *    - Effect: Paused→onExit
 *
 * 4. **Level Complete**:
 *    - Level: changeState(VictoryState())
 *    - Stack: [Menu, Victory]
 *    - Effect: Level→onExit, Victory→onEnter
 *
 * 5. **Return to Menu**:
 *    - Any: clearAndPushState(MenuState())
 *    - Stack: [Menu]
 *    - Effect: Pop all, then push Menu
 *
 * Deferred Transitions (Critical):
 * - Transitions requested during update/render stored as pending
 * - Applied at end of frame via processPendingChanges()
 * - Prevents crashes from state deletion during event handling
 *
 * Rendering Algorithm:
 * ```
 * Find first opaque state from top:
 *   for i from top to bottom:
 *     if !states[i+1].isTransparent():
 *       startIndex = i+1
 *
 * Render from startIndex to top:
 *   for i from startIndex to top:
 *     states[i].render(window)
 * ```
 * Example: [Menu, Level(opaque), Paused(transparent)]
 * → Render Level, then Paused
 *
 * Window Resize Propagation:
 * - Game calls onWindowResize(w, h)
 * - StateManager stores dimensions
 * - Only active state (top) notified immediately
 * - New states receive stored dimensions on push
 *
 * Empty Stack Handling:
 * - isEmpty() returns true when stack empty
 * - Game checks isEmpty() to exit application
 * - Occurs when user closes all menus or exits
 *
 * @see State for state interface
 * @see Game for state manager usage
 */
class StateManager {
public:
    StateManager();
    ~StateManager() = default;

    /**
     * @brief Push new state onto stack
     *
     * Sequence:
     * 1. Set state's stateManager pointer
     * 2. Call current state's onExit()
     * 3. Add new state to stack
     * 4. Notify new state of window size
     * 5. Call new state's onEnter()
     *
     * Used for: Opening overlays, starting levels
     *
     * @param state Unique ptr to new state (ownership transferred)
     */
    void pushState(std::unique_ptr<State> state);

    /**
     * @brief Remove current state from stack
     *
     * Sequence:
     * 1. Call current state's onExit()
     * 2. Remove from stack
     *
     * Used for: Closing overlays, going back
     */
    void popState();

    /**
     * @brief Replace current state with new state
     *
     * Sequence:
     * 1. Call current state's onExit()
     * 2. Remove current state
     * 3. Push new state (same as pushState)
     *
     * Used for: Level transitions
     *
     * @param state New state to activate
     */
    void changeState(std::unique_ptr<State> state);

    /**
     * @brief Remove all states from stack
     *
     * Pops all states in reverse order.
     * Used for: Exiting to desktop
     */
    void clearStates();

    /**
     * @brief Clear all states then push new state
     *
     * Deferred operation (via pending system).
     * Used for: Returning to menu from anywhere
     *
     * @param state New root state
     */
    void clearAndPushState(std::unique_ptr<State> state);

    /**
     * @brief Check if state stack is empty
     *
     * Used by Game to detect application exit condition.
     *
     * @return true if no states active
     */
    [[nodiscard]] bool isEmpty() const { return states.empty(); }

    /**
     * @brief Get top state (currently active)
     *
     * @return Pointer to active state, or nullptr if empty
     */
    [[nodiscard]] State* getCurrentState() const;

    /**
     * @brief Forward events to active state
     *
     * Only top state handles input.
     *
     * @param window SFML window for input
     */
    void handleEvents(sf::RenderWindow& window);

    /**
     * @brief Update active state logic
     *
     * Only top state updates.
     *
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);

    /**
     * @brief Render visible states (respecting transparency)
     *
     * Algorithm:
     * 1. Find first opaque state from top
     * 2. Render from that state to top
     *
     * @param window SFML window to draw to
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief Notify states of window resize
     *
     * Stores new dimensions and notifies active state.
     * New states pushed later receive stored dimensions.
     *
     * @param width New window width
     * @param height New window height
     */
    void onWindowResize(float width, float height);

    /**
     * @brief Apply pending state transitions
     *
     * CRITICAL: Must be called at end of each frame.
     * Prevents state deletion during event handling.
     *
     * Called by: Game::run() after render, before next frame
     */
    void processPendingChanges();

private:
    std::vector<std::unique_ptr<State>> states; // State stack (back = top)

    // Deferred transition system
    enum class PendingAction { None, Push, Pop, Change, Clear, ClearAndPush };
    PendingAction pendingAction = PendingAction::None;
    std::unique_ptr<State> pendingState = nullptr;

    // Stored window dimensions for new states
    float currentWindowWidth = 800.0f;
    float currentWindowHeight = 860.0f;
};

} // namespace representation

#endif