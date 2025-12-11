#include "../../include/logic/patterns/Subject.h"
#include <algorithm>

void Subject::attach(const std::shared_ptr<Observer>& o) {
  if (o) {
    observers.push_back(o);
  }
}

void Subject::detach(const std::shared_ptr<Observer>& o) {
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

void Subject::notify(GameEvent event) {
  observers.erase(
    std::remove_if(observers.begin(), observers.end(),
      [&event](const std::weak_ptr<Observer>& weak) {
        if (auto observer = weak.lock()) {
          observer->update(event);
          return false;
        }
        return true;
      }),
    observers.end()
  );
}