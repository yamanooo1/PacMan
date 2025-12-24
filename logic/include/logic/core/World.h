#ifndef PACMAN_WORLD_H
#define PACMAN_WORLD_H
#include "AbstractFactory.h"
#include "EntityModel.h"
#include <memory>
#include <vector>

namespace logic {

class PacMan;
class Ghost;
enum class GhostType;
class Wall;
class Coin;
class Fruit;
class Score;
class Lives;

/**
 * @brief Central game world manager - coordinates all game entities and systems
 *
 * World is the main game logic hub that manages:
 * - Entity lifecycle (creation, update, deletion)
 * - Collision detection between PacMan and all entities
 * - Fear mode state (activated by fruit collection)
 * - Death animation and respawn sequences
 * - Ready state (countdown before gameplay starts)
 * - Level completion detection
 * - Map loading from text files
 *
 * Architecture:
 * - Pure logic class (namespace logic::)
 * - No rendering - uses AbstractFactory to create entities + views
 * - Factory pattern bridges logic ↔ representation layers
 *
 * Game Loop Structure:
 * ```
 * World::update(deltaTime)
 *   ├─ if readyState → countdown timer
 *   ├─ if deathAnimation → countdown, then respawn
 *   ├─ if levelCleared → display message
 *   ├─ else normal gameplay:
 *   │   ├─ updatePacMan() - movement with collision checking
 *   │   ├─ updateGhosts() - AI updates for each ghost
 *   │   ├─ updateFearMode() - fear timer and transitions
 *   │   ├─ checkCollisions() - detect overlaps
 *   │   ├─ removeDeadEntities() - garbage collection
 *   │   └─ checkLevelComplete() - win condition
 * ```
 *
 * State Management:
 * - Ready state: Initial countdown (4.7s) before level starts
 * - Death animation: PacMan dies → animation (2.0s) → respawn
 * - Fear mode: Ghosts vulnerable (7s scaled by level) → return to normal
 * - Level cleared: All collectables gone → display message (3s) → next level
 *
 * Difficulty Scaling (per level):
 * - Ghost speed: 1.0 + (level-1) * 0.09 (9% faster per level)
 * - Fear duration: 1.0 - (min(level,6)-1) * 0.09 (9% shorter, caps at level 6)
 * - Example level 5: ghosts 36% faster, fear mode 36% shorter
 *
 * Map Format:
 * ```
 * 21 21        // width height
 * xxxxx...     // x=wall, .=coin, f=fruit
 * x.P.....     // P=pacman spawn
 * x..r.b..     // r=red ghost, b=cyan, o=orange, p=pink
 * xxxwxxx      // w=ghost exit marker
 * ```
 *
 * Coordinate System:
 * - Grid-based: position 10.0 = center of grid cell 10
 * - Entity positions are float for smooth sub-grid movement
 * - Collision uses grid cell overlap detection
 *
 * @see ConcreteFactory for entity creation and view attachment
 */
class World {
private:
    // Entity management
    std::vector<std::unique_ptr<EntityModel>> entities;  // All game entities
    std::vector<Ghost*> ghosts;  // Quick access to ghost pointers (subset of entities)

    // Map properties
    int mapWidth;   // Grid width (typically 21)
    int mapHeight;  // Grid height (typically 21)

    // Difficulty scaling
    int currentLevel;              // Current level number (1-based)
    float ghostSpeedMultiplier;    // Speed increase per level
    float fearDurationMultiplier;  // Fear duration decrease per level

    // Factory for entity creation (bridges logic ↔ representation)
    AbstractFactory* factory;

    // Key entity references
    PacMan* pacman;  // Player entity (raw pointer into entities vector)

    // System references (owned by LevelState)
    std::shared_ptr<Score> score;  // Points tracking and high scores
    std::shared_ptr<Lives> lives;  // Life counter and game over detection

    // Spawn positions (for respawn after death)
    float pacmanSpawnX, pacmanSpawnY;
    std::vector<std::pair<float, float>> ghostSpawnPositions;

    // Fear mode state
    bool fearModeActive;      // True when ghosts are vulnerable
    float fearModeTimer;      // Time remaining in fear mode
    float fearModeDuration;   // Total fear duration (scaled by level)

    // Death animation state
    bool deathAnimationActive;  // True during PacMan death sequence
    float deathAnimationTimer;  // Time remaining in animation
    float deathAnimationDuration;  // Total animation length (2.0s)

    // Ready state (pre-game countdown)
    bool readyStateActive;   // True during "READY!" message
    float readyStateTimer;   // Time remaining before gameplay
    float readyStateDuration;  // Total countdown (4.7s)

    // Level completion state
    bool levelCleared;               // True when all collectables collected
    bool levelClearedDisplayActive;  // True during "LEVEL CLEARED!" message
    float levelClearedDisplayTimer;  // Time remaining for message display
    float levelClearedDisplayDuration;  // Display duration (3.0s)

    // Ghost spawn area exits (positions marked 'w' in map)
    std::vector<std::pair<int, int>> exitPositions;

    // Internal update methods (called by main update loop)

    /**
     * @brief Update PacMan with advanced movement and collision prediction
     *
     * Movement system features:
     * - Desired direction buffering (stores player input)
     * - Perpendicular turn validation (only at grid centers)
     * - Wall collision prediction (stops before hitting wall)
     * - Smooth grid alignment for clean turns
     *
     * Algorithm:
     * 1. Check if player wants to change direction (desired != current)
     * 2. Validate turn: no wall AND (aligned with grid OR not perpendicular)
     * 3. If valid: snap to grid center, execute turn
     * 4. Check wall ahead in current direction
     * 5. If wall close: stop at cell center, else continue moving
     *
     * @param deltaTime Time since last update
     */
    void updatePacMan(float deltaTime) const;

    /**
     * @brief Update all ghosts (AI and movement)
     *
     * Calls Ghost::update() for each ghost which handles:
     * - State machine (WAITING/EXITING/CHASING/FEAR)
     * - Pathfinding decisions at intersections
     * - Movement execution
     *
     * @param deltaTime Time since last update
     */
    void updateGhosts(float deltaTime);

    /**
     * @brief Check for collisions between PacMan and all other entities
     *
     * Collision types:
     * - Coin: Collect (10-30 points, marked dead)
     * - Fruit: Collect (50 points) + activate fear mode
     * - Ghost (normal): PacMan dies
     * - Ghost (fear): Ghost dies (+200 points, respawn)
     * - Wall: No collision (prevented by movement logic)
     *
     * Detection method: AABB (axis-aligned bounding box)
     * ```
     * collision = (x1 < x2+w2) && (x1+w1 > x2) &&
     *             (y1 < y2+h2) && (y1+h1 > y2)
     * ```
     *
     * Entity type identification by size:
     * - Coin: 0.02x0.02
     * - Fruit: 0.03x0.03
     * - Ghost: 0.8x0.8
     * - Wall: 1.0x1.0 (ignored in collision checks)
     */
    void checkCollisions();

    /**
     * @brief Remove dead entities from entity list and views
     *
     * Two-phase cleanup:
     * 1. Factory removes dead views (representation layer)
     * 2. World removes dead entities (logic layer)
     *
     * Uses erase-remove idiom for efficient vector cleanup.
     */
    void removeDeadEntities();

    /**
     * @brief Check if level is complete (all collectables collected)
     *
     * Win condition:
     * - Count remaining coins (0.02x0.02) and fruits (0.03x0.03)
     * - If count == 0 → level cleared
     *
     * Triggers:
     * - Sets levelCleared flag
     * - Starts levelClearedDisplay timer
     * - Notifies Score of LEVEL_CLEARED (+1000 points)
     *
     * Transition:
     * - After display timer expires → LevelState creates next level
     */
    void checkLevelComplete();

public:
    /**
     * @brief Construct world for specific level
     *
     * @param f Factory for creating entities (bridges logic ↔ representation)
     * @param level Current level number (1-based, affects difficulty)
     *
     * Difficulty calculation:
     * - ghostSpeedMultiplier = 1.0 + (level-1) * 0.09
     * - fearDurationMultiplier = 1.0 - (min(level,6)-1) * 0.09
     * - fearModeDuration = 7.0 * fearDurationMultiplier
     */
    explicit World(AbstractFactory* f, int level = 1);

    // Map configuration
    void setMapDimensions(int w, int h);

    // System attachments
    void setScore(const std::shared_ptr<Score>& s) { score = s; }
    void setLives(const std::shared_ptr<Lives>& l) { lives = l; }

    // Level state queries
    [[nodiscard]] bool isLevelCleared() const { return levelCleared; }
    [[nodiscard]] bool isLevelClearedDisplayActive() const { return levelClearedDisplayActive; }

    // Entity management
    void addEntity(std::unique_ptr<EntityModel> entity);

    /**
     * @brief Create PacMan entity with centered positioning and observer setup
     *
     * Process:
     * 1. Factory creates PacMan model + view
     * 2. Center in grid cell (position adjustment for 0.8 size in 1.0 cell)
     * 3. Store spawn position for respawns
     * 4. Attach Lives observer (for death notifications)
     * 5. Add to entity list
     *
     * @param x Grid X position (typically from 'P' marker in map)
     * @param y Grid Y position
     */
    void createPacMan(float x, float y);

    /**
     * @brief Create Ghost entity with AI personality and difficulty scaling
     *
     * @param x Grid X spawn position
     * @param y Grid Y spawn position
     * @param type AI personality (RANDOM/CHASER/AMBUSHER)
     * @param color Visual identifier (RED/PINK/CYAN/ORANGE)
     * @param waitTime Seconds to wait before exiting spawn (stagger timing)
     */
    void createGhost(float x, float y, GhostType type, GhostColor color, float waitTime);

    void createWall(float x, float y, float w, float h);
    void createCoin(float x, float y);
    void createFruit(float x, float y);

    /**
     * @brief Load map from text file and populate world
     *
     * Map format:
     * ```
     * 21 21           // Width Height
     * xxxxxxxxxxxxx   // Row of tiles
     * x.........x     // . = coin
     * xfxx...xxfx     // f = fruit (power pellet)
     * x.P.......x     // P = PacMan spawn
     * x.r.b.o.p.x     // r/b/o/p = ghost spawns (red/cyan/orange/pink)
     * xxxwwwwxxxx     // w = ghost exit marker
     * ```
     *
     * Tile mapping:
     * - 'x': Wall (1.0x1.0)
     * - '.': Coin (0.02x0.02)
     * - 'f': Fruit (0.03x0.03)
     * - 'P': PacMan spawn point
     * - 'r': Red ghost (CHASER, 0s wait)
     * - 'p': Pink ghost (RANDOM, 10s wait)
     * - 'b': Cyan ghost (AMBUSHER, 0.6s wait)
     * - 'o': Orange ghost (CHASER, 5s wait)
     * - 'w': Ghost exit marker (stored in exitPositions)
     *
     * @param filename Path to map file (e.g., "resources/map/map1.txt")
     * @return true if loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filename);

    [[nodiscard]] bool isFearModeActive() const { return fearModeActive; }

    // Map accessors
    [[nodiscard]] int getMapWidth() const { return mapWidth; }
    [[nodiscard]] int getMapHeight() const { return mapHeight; }
    [[nodiscard]] const std::vector<std::unique_ptr<EntityModel>>& getEntities() const { return entities; }
    [[nodiscard]] size_t getEntityCount() const { return entities.size(); }

    /**
     * @brief Main world update - game loop coordinator
     *
     * Update priority (first matching state executes):
     * 1. Ready state: Countdown before gameplay
     * 2. Death animation: PacMan death sequence
     * 3. Level cleared display: Victory message
     * 4. Normal gameplay: Full update cycle
     *
     * @param deltaTime Time since last frame (seconds)
     */
    void update(float deltaTime);

    [[nodiscard]] PacMan* getPacMan() const { return pacman; }

    /**
     * @brief Reset PacMan and all ghosts to spawn positions
     *
     * Called after death animation completes.
     * Resets:
     * - PacMan position and direction (LEFT)
     * - Ghost positions to spawn points
     * - Ghost directions (UP)
     * - Ghost spawn flags (allows re-exit)
     * - Exits fear mode if active
     */
    void respawnPacManAndGhosts();

    /**
     * @brief Check if rectangular area overlaps with any wall
     *
     * Used for collision prediction (less commonly than hasWallInGridCell).
     *
     * @param x Area X position
     * @param y Area Y position
     * @param width Area width
     * @param height Area height
     * @return true if overlaps with wall, false otherwise
     */
    [[nodiscard]] bool isWall(float x, float y, float width, float height) const;

    /**
     * @brief Check if specific grid cell contains a wall
     *
     * Primary collision detection method (faster than isWall).
     *
     * Algorithm:
     * - Iterate entities, filter by size (width/height >= 0.9)
     * - Check if wall's grid position matches query position
     * - Grid position: floor(entity.x), floor(entity.y)
     *
     * @param gridX Grid column (integer)
     * @param gridY Grid row (integer)
     * @return true if cell contains wall, false otherwise
     */
    [[nodiscard]] bool hasWallInGridCell(int gridX, int gridY) const;

    /**
     * @brief Activate fear mode (triggered by fruit collection)
     *
     * Effects:
     * - Sets fearModeActive flag
     * - Resets fearModeTimer to full duration
     * - Calls Ghost::enterFearMode() for all ghosts
     * - Ghosts turn blue, flee from PacMan, become vulnerable
     *
     * Called by checkCollisions() when PacMan collects fruit.
     */
    void activateFearMode();

    [[nodiscard]] float getGhostSpeedMultiplier() const { return ghostSpeedMultiplier; }

    /**
     * @brief Update fear mode timer and handle transitions
     *
     * Timer states:
     * - 7.0s → 2.0s: Normal fear mode (solid blue)
     * - 2.0s → 0.0s: Warning phase (flashing blue/white)
     * - 0.0s: Exit fear mode (return to normal)
     *
     * @param deltaTime Time since last update
     */
    void updateFearMode(float deltaTime);

    /**
     * @brief Check if fear mode is in warning phase (last 2 seconds)
     *
     * Used by GhostView to trigger flashing animation.
     *
     * @return true if fear mode active and timer <= 2.0s
     */
    [[nodiscard]] bool isFearModeEnding() const;

    // Death animation control
    void startDeathAnimation();
    void updateDeathAnimation(float deltaTime);
    [[nodiscard]] bool isDeathAnimationActive() const { return deathAnimationActive; }

    // Ready state control
    void startReadyState();
    void updateReadyState(float deltaTime);
    [[nodiscard]] bool isReadyStateActive() const { return readyStateActive; }

    // Ghost exit positions
    [[nodiscard]] bool isExitPosition(int gridX, int gridY) const;
    [[nodiscard]] std::vector<std::pair<int, int>> getExitPositions() const;
};

} // namespace logic

#endif