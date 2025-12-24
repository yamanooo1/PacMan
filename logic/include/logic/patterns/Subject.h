#ifndef PACMAN_SUBJECT_H
#define PACMAN_SUBJECT_H
#include "Observer.h"
#include <memory>
#include <vector>

namespace logic {

/**
 * @brief Subject base class for the Observer design pattern
 *
 * Design Pattern: Subject (Observable)
 * - Maintains a list of observers
 * - Notifies all observers when events occur
 * - Uses weak_ptr to avoid circular dependencies and memory leaks
 *
 * Memory Management:
 * - Observers stored as weak_ptr (subject doesn't own observers)
 * - Automatic cleanup of expired weak_ptrs during notify()
 * - Prevents dangling pointers when observers are destroyed
 *
 * Usage Pattern:
 * 1. Entity inherits from Subject (via EntityModel)
 * 2. View/Score/Lives created as shared_ptr<Observer>
 * 3. attach(observer) subscribes observer to subject
 * 4. notify(event) informs all active observers
 * 5. Observers automatically removed when destroyed
 *
 * Example:
 * ```cpp
 * auto pacman = std::make_unique<PacMan>(x, y);
 * auto view = std::make_shared<PacManView>(pacman.get(), ...);
 * pacman->attach(view);  // View now observes PacMan
 * pacman->die();         // Internally calls notify(PACMAN_DIED)
 * // view->update(PACMAN_DIED) automatically triggered
 * ```
 *
 * @see Observer for the observer interface
 */
class Subject {
private:
    /**
     * @brief List of observers (as weak_ptr to avoid ownership)
     *
     * weak_ptr chosen because:
     * - Subject doesn't own observers (ConcreteFactory owns views)
     * - Prevents circular dependencies (observer → subject ← observer)
     * - Auto-cleanup when observers destroyed
     */
    std::vector<std::weak_ptr<Observer>> observers;

public:
    /**
     * @brief Subscribe an observer to this subject
     *
     * @param o Shared pointer to observer (must outlive notifications)
     *
     * @note Safe to call multiple times with same observer (duplicates allowed)
     * @note Observer remains subscribed until explicitly detached or destroyed
     */
    void attach(const std::shared_ptr<Observer>& o);

    /**
     * @brief Unsubscribe an observer from this subject
     *
     * @param o Observer to remove
     *
     * @note Safe to call even if observer not attached
     * @note Automatically removes expired weak_ptrs during cleanup
     */
    void detach(const std::shared_ptr<Observer>& o);

    virtual ~Subject() = default;

protected:
    /**
     * @brief Notify all observers of a game event
     *
     * Iterates through all attached observers and calls update(event).
     * Automatically removes expired weak_ptrs (destroyed observers).
     *
     * @param event The event type to broadcast
     *
     * @note Called by derived classes when state changes occur
     * @note Thread-safe if observers handle events synchronously
     */
    void notify(GameEvent event);
};

} // namespace logic

#endif