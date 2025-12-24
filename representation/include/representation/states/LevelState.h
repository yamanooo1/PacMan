#ifndef LEVELSTATE_H
#define LEVELSTATE_H

#include "../../../../logic/include/logic/core/World.h"
#include "../../../../logic/include/logic/systems/Lives.h"
#include "../../../../logic/include/logic/systems/Score.h"
#include "../core/Camera.h"
#include "../systems/HUD.h"
#include "State.h"
#include <memory>
#include <string>

namespace representation {

class ConcreteFactory;

/**
 * @brief LevelState - active gameplay state
 *
 * ARCHITECTURE: Main gameplay coordinator
 *
 * Components:
 * 1. **World** (logic::World): Game simulation
 * 2. **Factory** (ConcreteFactory): Entity creation & rendering
 * 3. **Camera**: Grid-to-screen transformation
 * 4. **HUD**: Score, lives, level display
 * 5. **Score** (logic::Score): Point tracking
 * 6. **Lives** (logic::Lives): Life counter
 *
 * Initialization Sequence:
 * ```
 * Constructor:
 *   Store level number and starting score
 *
 * First render() call:
 *   1. Create camera (grid-to-screen)
 *   2. Create score system (with starting score)
 *   3. Create lives system (3 lives)
 *   4. Create factory (model+view creation)
 *   5. Load sprites from sprite sheet
 *   6. Attach sound observer to factory
 *   7. Create HUD for UI
 *   8. Create world (pass factory)
 *   9. Attach score and lives to world
 *   10. Load map from file (create entities)
 *   11. Start "READY!" state (4.7s delay)
 *
 * Why render()?
 *   Requires factory to exist before world creation.
 *   Factory needs window reference (available in render).
 * ```
 *
 * Game Loop (via update):
 * ```
 * Pause check → return if paused
 *
 * Sound management:
 *   - Special states (death, ready, cleared) → stop movement sound
 *   - Normal gameplay → movement sound when:
 *     * PacMan actually moving (position changed)
 *     * Has direction set
 *     * Coin collected < 0.3s ago
 *     * Coins still remaining
 *
 * World update:
 *   - Updates all entities
 *   - Handles collisions
 *   - Checks win/lose conditions
 *
 * State transitions:
 *   - Level cleared → VictoryState(nextLevel, score)
 *   - Game over → VictoryState(0, score)
 * ```
 *
 * Input Handling:
 * - Arrow Keys / WASD: Set PacMan desired direction
 * - ESC: Push PausedState
 * - Input forwarded to World via PacMan model
 *
 * Window Layout:
 * ```
 * ┌─────────────────────────┐
 * │                         │
 * │      GAME AREA          │ ← 800px (adjusted for window)
 * │   (21×21 grid cells)    │
 * │                         │
 * ├─────────────────────────┤
 * │   HUD (60px fixed)      │ ← Score | Level | Lives
 * └─────────────────────────┘
 * Total height: gameArea + 60
 * ```
 *
 * Movement Sound Logic (Complex):
 * - Triggered when PacMan moves AND eats coins
 * - Stops when:
 *   * No movement
 *   * >0.3s since last coin
 *   * No coins remaining
 *   * Special state active (death, etc.)
 * - Uses coin counting before/after update
 * - Tracks time since last coin collection
 *
 * @see logic::World for game simulation
 * @see ConcreteFactory for entity creation
 */
class LevelState : public State {
public:
    /**
     * @brief Initialize LevelState
     *
     * @param level Level number (1, 2, 3...)
     * @param startingScore Score carried from previous level (0 for level 1)
     */
    explicit LevelState(int level, int startingScore = 0);
    ~LevelState() override;

    void onEnter() override;
    void onExit() override;
    void onWindowResize(float width, float height) override;
    void handleEvents(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    /**
     * @brief Initialize game systems (called on first render)
     *
     * Creates camera, score, lives, returns success.
     *
     * @return true if successful
     */
    bool loadLevel();

    /**
     * @brief Read keyboard input and set PacMan direction
     *
     * Maps arrow keys / WASD to Direction enum.
     */
    void handleInput();

    std::shared_ptr<logic::Lives> lives;
    std::shared_ptr<logic::Score> score;
    std::shared_ptr<Camera> camera;
    std::unique_ptr<HUD> hud;
    std::unique_ptr<logic::World> world;
    std::unique_ptr<ConcreteFactory> factory;

    int currentLevel;
    int initialScore;
    std::string mapFile; // "../../resources/map/map1.txt"
    bool pauseRequested;

    float windowWidth;
    float windowHeight;
    static constexpr float HUD_HEIGHT = 60.0f;
};

} // namespace representation

#endif