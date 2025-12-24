#ifndef PACMAN_COIN_H
#define PACMAN_COIN_H
#include "Collectable.h"

namespace logic {

/**
 * @brief Small collectible pellets scattered throughout the maze
 *
 * Coins are the primary collectible in PacMan, represented as tiny dots.
 *
 * Scoring System:
 * - Base value: 30 points
 * - Decay system: Points decrease with time since last coin
 * - Formula: 30 * (0.9^(timeSince/0.5))
 * - Example: Collect coins quickly = 30, 27, 25... (momentum bonus)
 * - Example: Wait between coins = 30, 15, 10... (slow collection penalty)
 *
 * Visual Representation:
 * - Size: 0.02x0.02 grid units (tiny dots)
 * - Color: White (CoinView renders as small circles)
 * - Sprite: Small dot from sprite atlas
 *
 * Map Notation:
 * - '.' in map file creates a coin at that position
 *
 * @see Score::update() for decay calculation implementation
 */
class Coin : public Collectable {
public:
    /**
     * @brief Construct a coin at grid position
     *
     * Standard size: 0.02x0.02 (consistent for all coins)
     */
    Coin(float x, float y);
    ~Coin() override = default;

    /**
     * @brief Notify coin collection event
     *
     * Triggers COIN_COLLECTED event which:
     * - Updates score with decay calculation
     * - Plays eating sound (looped while collecting)
     * - Resets decay timer
     */
    void onCollected() override;
};

} // namespace logic

#endif