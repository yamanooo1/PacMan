//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_OBSERVER_H
#define PACMAN_OBSERVER_H


enum class GameEvent {
  COIN_COLLECTED,
  GHOST_EATEN,
  FRUIT_COLLECTED,
  PACMAN_DIED,
  LEVEL_CLEARED
};

class Observer {
public:
  virtual ~Observer() = default;  // Virtual destructor

  // This is THE method that gets called when Subject changes
  virtual void update(GameEvent event) = 0;  // Pure virtual = todo must be implemented
};


#endif //PACMAN_OBSERVER_H