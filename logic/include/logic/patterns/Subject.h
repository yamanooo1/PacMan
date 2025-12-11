#ifndef PACMAN_SUBJECT_H
#define PACMAN_SUBJECT_H
#include "Observer.h"
#include <memory>
#include <vector>

class Subject {
private:
  std::vector<std::weak_ptr<Observer>> observers;

public:
  void attach(const std::shared_ptr<Observer>& o);
  void detach(const std::shared_ptr<Observer>& o);
  virtual ~Subject() = default;

protected:
  void notify(GameEvent event);
};

#endif