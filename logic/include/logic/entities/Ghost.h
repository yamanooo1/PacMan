#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H
#include "../core/EntityModel.h"
#include <vector>

namespace logic {

class World;
class PacMan;

/**
 * @brief Ghost color identification
 *
 * Colors match classic PacMan arcade:
 * - RED (Blinky): Aggressive chaser
 * - PINK (Pinky): Ambusher who targets ahead
 * - CYAN (Inky): Complex behavior, attacks from side
 * - ORANGE (Clyde): Random/unpredictable
 */
enum class GhostColor { RED, PINK, CYAN, ORANGE };

/**
 * @brief Ghost AI personality types
 *
 * Defines targeting behavior for ghost movement:
 * - RANDOM: Picks random valid directions (50/50 new vs current)
 * - AMBUSHER: Targets 4 cells ahead of PacMan
 * - CHASER: Directly targets PacMan's current position
 */
enum class GhostType { RANDOM, AMBUSHER, CHASER };

/**
 * @brief Ghost state machine states
 *
 * State transitions:
 * WAITING → EXITING → CHASING ⇄ FEAR
 *    ↓         ↓
 *  timer    exit pos
 *
 * - WAITING: Initial state, counting down wait timer in spawn area
 * - EXITING: Navigating to exit position (marked 'w' in map)
 * - CHASING: Normal gameplay, hunting PacMan based on type
 * - FEAR: Vulnerable state, running away from PacMan (blue color)
 */
enum class GhostState { WAITING, EXITING, CHASING, FEAR };

/**
 * @brief Enemy entity with AI-driven movement and state machine
 *
 * Ghosts are the primary antagonists that hunt PacMan using various strategies.
 * Each ghost has unique personality (type), appearance (color), and timing (wait).
 *
 * AI System:
 * - Intersection-based decision making (only choose direction at junctions)
 * - Manhattan distance targeting (straight-line approximation)
 * - Look-ahead for ambush behavior
 * - Random elements for unpredictability
 *
 * State Machine:
 * 1. WAITING: Count down timer, don't move
 * 2. EXITING: Navigate to exit marker, avoid turning back to spawn
 * 3. CHASING: Hunt PacMan based on personality type
 * 4. FEAR: Run away from PacMan (activated by fruit collection)
 *
 * Speed Scaling (difficulty progression):
 * - Normal speed: 2.5 * speedMultiplier (increases per level)
 * - Fear speed: 1.5 (constant, slower when vulnerable)
 * - speedMultiplier = 1.0 + (level-1) * 0.09 (e.g., level 5 = 1.36x)
 *
 * Collision Behavior:
 * - In normal mode: PacMan dies
 * - In fear mode: Ghost dies (+200 points), respawns at spawn
 *
 * Observer Pattern:
 * - Notifies GHOST_EATEN when killed in fear mode
 */
class Ghost : public EntityModel {
private:
    GhostType type;              // AI personality (RANDOM/AMBUSHER/CHASER)
    GhostColor color;            // Visual identifier (RED/PINK/CYAN/ORANGE)
    GhostState state;            // Current state machine state

    float waitTimer;             // Time remaining before exiting spawn
    float originalWaitTime;      // Initial wait time (for respawn)

    float speed;                 // Current movement speed
    float normalSpeed;           // Speed during CHASING state
    float fearSpeed;             // Speed during FEAR state (slower)

    int lastDecisionGridX;       // Last grid position where direction was chosen
    int lastDecisionGridY;       // (prevents constant recalculation at same spot)

    bool hasLeftSpawn;           // True after first exit (prevents re-entering spawn)
    bool fearModeEnding;         // True in last 2 seconds of fear mode (flashing)
    bool shouldEnterFearWhenLeaving; // Queued fear mode activation after exiting

    /**
     * @brief Get all valid movement directions at current position
     *
     * Rules:
     * - No turning back (opposite direction blocked unless forced)
     * - No walls
     * - No re-entering spawn area (once left)
     * - Allow opposite direction only if no other options
     *
     * @return Vector of valid directions, empty if stuck
     */
    std::vector<Direction> getViableDirections(int gridX, int gridY, const World* world) const;

    /**
     * @brief Check if current position is an intersection
     *
     * Intersection = position with 2+ viable directions (decision point)
     * Ghosts only reconsider direction at intersections for smooth movement.
     *
     * @return true if multiple paths available
     */
    bool isAtIntersection(int gridX, int gridY, World* world) const;

    /**
     * @brief Choose next direction based on AI type and state
     *
     * Behavior varies by type:
     * - CHASER: Target PacMan directly (minimize manhattan distance)
     * - AMBUSHER: Target 4 cells ahead of PacMan's direction
     * - RANDOM: 50% random, 50% continue current direction
     * - FEAR: Maximize distance from PacMan (run away)
     *
     * @param gridX Current ghost grid X position
     * @param gridY Current ghost grid Y position
     * @param world World for wall checking
     * @param pacman PacMan for targeting
     * @return Best direction based on strategy
     */
    Direction chooseNextDirection(int gridX, int gridY, const World* world, const PacMan* pacman) const;

    /**
     * @brief Choose direction that leads to exit position
     *
     * Used during EXITING state to navigate from spawn to 'w' marker.
     * Uses manhattan distance to find nearest exit marker.
     *
     * @return Direction that moves toward closest exit
     */
    Direction chooseDirectionToExit(int gridX, int gridY, const World* world) const;

    /**
     * @brief Calculate manhattan distance between two grid positions
     *
     * Manhattan distance = |x1-x2| + |y1-y2| (grid-based pathfinding)
     * Used for targeting and exit finding.
     *
     * @return Distance in grid cells
     */
    [[nodiscard]] static int manhattanDistance(int x1, int y1, int x2, int y2);

    /**
     * @brief Check if position is inside spawn area
     *
     * Spawn area: grid cells (7-11, 8-10) - center of map
     * Used to prevent ghosts from re-entering after leaving.
     *
     * @return true if inside spawn rectangle
     */
    [[nodiscard]] static bool isInSpawnArea(int gridX, int gridY);

public:
    /**
     * @brief Construct a ghost with specific personality and timing
     *
     * @param x Initial X position (spawn point)
     * @param y Initial Y position (spawn point)
     * @param t AI type (personality)
     * @param c Visual color
     * @param waitTime Seconds to wait before leaving spawn (stagger timing)
     * @param speedMultiplier Level difficulty multiplier (1.0 + level scaling)
     */
    Ghost(float x, float y, GhostType t, GhostColor c, float waitTime, float speedMultiplier = 1.0f);
    ~Ghost() override = default;

    // Type and state accessors
    [[nodiscard]] GhostType getType() const { return type; }
    [[nodiscard]] GhostState getState() const { return state; }
    [[nodiscard]] GhostColor getColor() const { return color; }

    /**
     * @brief Activate fear mode (ghost becomes vulnerable)
     *
     * Triggered by fruit collection in World.
     * - Changes to blue color in view
     * - Reverses direction immediately
     * - Runs away from PacMan
     * - Can be eaten for 200 points
     *
     * Special cases:
     * - If WAITING/EXITING: queues fear mode for after exit
     * - If already FEAR: resets timer (fruit extends fear mode)
     */
    void enterFearMode();

    /**
     * @brief Exit fear mode (return to normal hunting)
     *
     * Called when fear timer expires (~7 seconds, scaled by level).
     * Returns to CHASING state with normal speed and color.
     */
    void exitFearMode();

    [[nodiscard]] bool isInFearMode() const;
    [[nodiscard]] bool shouldShowFearMode() const;

    /**
     * @brief Reset ghost to initial waiting state
     *
     * Called on level start and after PacMan death.
     * Resets hasLeftSpawn flag so ghost stays in spawn area.
     */
    void resetSpawnFlag();

    /**
     * @brief Respawn after being eaten in fear mode
     *
     * Sets state to EXITING to navigate back out of spawn area.
     * Skips WAITING state (immediate exit after death).
     */
    void respawnAfterEaten();

    /**
     * @brief Notify observers that ghost was eaten
     *
     * Triggers GHOST_EATEN event for:
     * - Score: +200 points
     * - Sound: Ghost eaten sound effect
     */
    void onEaten();

    /**
     * @brief Update ghost AI and position
     *
     * State machine update:
     * - WAITING: Decrement timer, transition to EXITING when ready
     * - EXITING: Navigate to exit marker
     * - CHASING/FEAR: Make decisions at intersections, move smoothly
     *
     * Decision making:
     * - Only reconsider direction at intersections or when blocked
     * - Cache last decision position to avoid recalculating
     * - Smooth movement between decisions
     *
     * @param deltaTime Time since last update
     * @param world World for collision checking
     * @param pacman PacMan for targeting
     */
    void update(float deltaTime, World* world, const PacMan* pacman);

    // Fear mode UI state
    [[nodiscard]] bool isFearModeEnding() const { return fearModeEnding; }
    void setFearModeEnding(bool ending) { fearModeEnding = ending; }
};

} // namespace logic

#endif