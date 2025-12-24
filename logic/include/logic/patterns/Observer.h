#ifndef PACMAN_OBSERVER_H
#define PACMAN_OBSERVER_H

namespace logic {

/**
 * @brief Game event enumeration for Observer notifications
 *
 * Events represent state changes in the game that observers need to react to.
 * Used by Subject::notify() to inform all attached observers.
 *
 * Event Categories:
 * - Scoring: COIN_COLLECTED, FRUIT_COLLECTED, GHOST_EATEN, LEVEL_CLEARED
 * - Game State: PACMAN_DIED, DIRECTION_CHANGED
 * - Audio Cues: WALL_HIT (for sound effects)
 */
enum class GameEvent {
    COIN_COLLECTED,    // Score +10-30 (decay based), play eating sound
    GHOST_EATEN,       // Score +200, play ghost eaten sound
    FRUIT_COLLECTED,   // Score +50, activate fear mode, play fruit sound
    PACMAN_DIED,       // Lives -1, trigger death animation, stop movement sound
    LEVEL_CLEARED,     // Score +1000, transition to next level
    DIRECTION_CHANGED, // Update PacMan animation direction
    WALL_HIT           // Stop movement, play wall hit sound (optional)
};

/**
 * @brief Observer interface for the Observer design pattern
 *
 * Design Pattern: Observer
 * - Subjects (EntityModel, World) notify observers of state changes
 * - Observers react to events without tight coupling to subjects
 *
 * Implementation in this project:
 * - EntityView (representation::) observes EntityModel (logic::) for rendering
 * - Score observes entities for score updates
 * - Lives observes PacMan for life management
 * - SoundObserver observes entities for audio cues
 *
 * Architecture Benefits:
 * - Decouples logic:: from representation:: (no SFML in logic layer)
 * - Multiple observers per subject (view + score + sound all observe same entity)
 * - Easy to add new observers without modifying subjects
 *
 * @see Subject for the other half of the Observer pattern
 */
class Observer {
public:
    virtual ~Observer() = default;

    /**
     * @brief React to a game event
     *
     * Called by Subject::notify() when an event occurs.
     * Each observer implements its own reaction to relevant events.
     *
     * @param event The type of event that occurred
     *
     * Example implementations:
     * - Score::update() adds points based on event type
     * - PacManView::update() triggers death animation on PACMAN_DIED
     * - SoundObserver::update() plays appropriate sound effect
     */
    virtual void update(GameEvent event) = 0;
};

} // namespace logic

#endif