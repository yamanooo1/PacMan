#ifndef PACMAN_SOUNDOBSERVER_H
#define PACMAN_SOUNDOBSERVER_H

#include "../../../../logic/include/logic/patterns/Observer.h"
#include "SoundManager.h"
#include <memory>

namespace representation {

/**
 * @brief SoundObserver - bridges game events to audio
 *
 * DESIGN PATTERN: Observer (listens to game events)
 *
 * Purpose:
 * - Observe entity events (coin collected, ghost eaten, etc.)
 * - Trigger appropriate sounds via SoundManager
 * - Decouple game logic from audio system
 *
 * Attachment Flow:
 * ```
 * ConcreteFactory creates entity model:
 *   1. Create logic::PacMan (model)
 *   2. Create PacManView (view)
 *   3. Attach view to model (visual updates)
 *   4. Attach SoundObserver to model (audio)
 *
 * Event propagation:
 *   Model: notify(COIN_COLLECTED)
 *     → View: update(event)  [visual feedback]
 *     → SoundObserver: update(event)  [audio feedback]
 * ```
 *
 * Event → Sound Mapping:
 * - COIN_COLLECTED: No sound (movement sound handles this)
 * - FRUIT_COLLECTED: Fruit sound + start fear mode sound
 * - GHOST_EATEN: Ghost eaten sound
 * - PACMAN_DIED: Death sound + stop fear mode
 * - LEVEL_CLEARED: No sound (handled by state)
 * - DIRECTION_CHANGED: No sound
 *
 * Attached Entities:
 * - PacMan: PACMAN_DIED, DIRECTION_CHANGED
 * - Ghosts: GHOST_EATEN
 * - Coins: COIN_COLLECTED
 * - Fruits: FRUIT_COLLECTED
 *
 * Shared Ownership:
 * - Implements enable_shared_from_this
 * - Required for Observer pattern (weak_ptr storage)
 * - ConcreteFactory holds shared_ptr
 * - Entities hold weak_ptr via Subject
 *
 * @see logic::Observer for observer interface
 * @see logic::Subject for notification mechanism
 * @see SoundManager for audio playback
 */
class SoundObserver : public logic::Observer, public std::enable_shared_from_this<SoundObserver> {
public:
    SoundObserver();
    ~SoundObserver() override = default;

    /**
     * @brief Handle game event and trigger sound
     *
     * Called by Subject::notify() when entity emits event.
     *
     * Event handling:
     * - FRUIT_COLLECTED: Play fruit sound + start fear music
     * - GHOST_EATEN: Play ghost eaten sound
     * - PACMAN_DIED: Play death sound + stop fear music
     * - Others: Ignored
     *
     * @param event Game event type
     */
    void update(logic::GameEvent event) override;
};

} // namespace representation

#endif