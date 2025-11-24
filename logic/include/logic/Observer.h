//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_OBSERVER_H
#define PACMAN_OBSERVER_H


class Observer {
public:
  virtual ~Observer() = default;  // Virtual destructor

  // This is THE method that gets called when Subject changes
  virtual void update() = 0;  // Pure virtual = must be implemented
};


#endif //PACMAN_OBSERVER_H