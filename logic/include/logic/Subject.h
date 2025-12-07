//
// Subject.h - UPDATED to use weak_ptr for observers
//

#ifndef PACMAN_SUBJECT_H
#define PACMAN_SUBJECT_H

#include <vector>
#include <memory>
#include <algorithm>
#include "Observer.h"

class Subject {
private:
  // ✅ Store weak_ptr to avoid circular dependencies and allow automatic cleanup
  std::vector<std::weak_ptr<Observer>> observers;

public:
  // ✅ Attach takes shared_ptr (caller owns the Observer)
  void attach(std::shared_ptr<Observer> o) {
    if (o) {
      observers.push_back(o);
    }
  }

  // ✅ Detach takes shared_ptr and removes matching weak_ptr
  void detach(std::shared_ptr<Observer> o) {
    if (!o) return;

    observers.erase(
      std::remove_if(observers.begin(), observers.end(),
        [&o](const std::weak_ptr<Observer>& weak) {
          auto locked = weak.lock();
          return !locked || locked == o;
        }),
      observers.end()
    );
  }

  virtual ~Subject() = default;

protected:
  // ✅ Notify automatically cleans up expired weak_ptrs
  void notify(GameEvent event) {
    // Remove expired observers while notifying active ones
    observers.erase(
      std::remove_if(observers.begin(), observers.end(),
        [&event](std::weak_ptr<Observer>& weak) {
          if (auto observer = weak.lock()) {
            observer->update(event);
            return false;  // Keep this observer
          }
          return true;  // Remove expired observer
        }),
      observers.end()
    );
  }
};

#endif //PACMAN_SUBJECT_H