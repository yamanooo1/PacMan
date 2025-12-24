#include "../../include/logic/patterns/Subject.h"
#include <algorithm>

namespace logic {

/**
 * @brief Add observer to notification list
 *
 * Stores observer as weak_ptr to avoid taking ownership.
 * Multiple attach() calls with same observer will create duplicates.
 */
void Subject::attach(const std::shared_ptr<Observer>& o) {
    if (o) {
        observers.push_back(o);
    }
}

/**
 * @brief Remove observer from notification list
 *
 * Uses remove_if + erase idiom to find and remove the observer.
 * Also removes any expired weak_ptrs encountered during search.
 *
 * @note Compares locked weak_ptr with target shared_ptr for equality
 */
void Subject::detach(const std::shared_ptr<Observer>& o) {
    if (!o)
        return;

    observers.erase(std::remove_if(observers.begin(), observers.end(),
                                   [&o](const std::weak_ptr<Observer>& weak) {
                                       auto locked = weak.lock();
                                       return !locked || locked == o; // Remove if expired or matches
                                   }),
                    observers.end());
}

/**
 * @brief Notify all living observers and cleanup dead ones
 *
 * Two-phase process:
 * 1. Try to lock() each weak_ptr
 * 2. If successful, call observer->update(event)
 * 3. If lock() fails (observer destroyed), mark for removal
 *
 * remove_if returns true for expired observers, automatically removing them.
 * This ensures the observer list stays clean without manual cleanup.
 *
 * @param event Event to broadcast to all observers
 *
 * @note The lambda captures event by reference for efficiency
 * @note Expired observers are removed in the same pass as notification
 */
void Subject::notify(GameEvent event) {
    observers.erase(std::remove_if(observers.begin(), observers.end(),
                                   [&event](const std::weak_ptr<Observer>& weak) {
                                       if (auto observer = weak.lock()) {
                                           observer->update(event); // Notify if still alive
                                           return false;            // Keep in list
                                       }
                                       return true; // Remove if expired
                                   }),
                    observers.end());
}

} // namespace logic