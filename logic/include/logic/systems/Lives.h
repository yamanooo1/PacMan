#ifndef PACMAN_LIVES_H
#define PACMAN_LIVES_H
#include "../patterns/Observer.h"
#include <memory>

namespace logic {

/**
 * @brief Life counter and game over detection system
 *
 * Lives tracks PacMan's remaining lives and detects game over condition.
 *
 * Lifecycle:
 * - Start: 3 lives (default)
 * - Death: -1 life per PACMAN_DIED event
 * - Game over: 0 lives remaining
 *
 * Observer Pattern:
 * - Observes: PacMan
 * - Event: PACMAN_DIED (triggered on ghost collision)
 * - Updates: Decrements life counter
 *
 * Usage:
 * ```cpp
 * auto lives = std::make_shared<Lives>(3);
 * pacman->attach(lives);
 *
 * // During gameplay:
 * if (lives->isGameOver()) {
 *     // Transition to game over state
 * }
 * ```
 *
 * Integration:
 * - World checks isGameOver() each frame
 * - LevelState displays lives in HUD
 * - VictoryState triggered when game over
 */
class Lives : public Observer, public std::enable_shared_from_this<Lives> {
private:
    int remainingLives; // Current life count (can be 0)
    int maxLives;       // Starting life count (for reset)

public:
    /**
     * @brief Initialize life counter
     *
     * @param startingLives Initial life count (default: 3)
     */
    explicit Lives(int startingLives = 3);
    ~Lives() override = default;

    /**
     * @brief Handle PACMAN_DIED event
     *
     * Decrements life counter on each death.
     * No lower bound - can go negative (checked by isGameOver).
     *
     * @param event Game event (only processes PACMAN_DIED)
     */
    void update(GameEvent event) override;

    /**
     * @brief Get remaining lives
     *
     * @return Current life count (0 = last life, < 0 shouldn't happen)
     */
    int getLives() const { return remainingLives; }

    /**
     * @brief Get maximum lives (starting count)
     *
     * @return Initial life count (for UI display)
     */
    int getMaxLives() const { return maxLives; }

    /**
     * @brief Check if game over condition met
     *
     * @return true if lives <= 0, false otherwise
     */
    bool isGameOver() const { return remainingLives <= 0; }

    /**
     * @brief Reset life counter (for new game)
     *
     * @param lives Life count to reset to (default: 3)
     */
    void reset(int lives = 3);
};

} // namespace logic

#endif