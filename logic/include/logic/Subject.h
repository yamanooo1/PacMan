#ifndef PACMAN_SUBJECT_H
#define PACMAN_SUBJECT_H
#include <vector>
#include <memory>
#include "Observer.h"

class Subject {
private:
  std::vector<std::weak_ptr<Observer>> observers;

public:
  void attach(std::shared_ptr<Observer> o);
  void detach(std::shared_ptr<Observer> o);
  virtual ~Subject() = default;

protected:
  void notify(GameEvent event);
};

#endif