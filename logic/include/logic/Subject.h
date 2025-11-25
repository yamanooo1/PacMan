//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_SUBJECT_H
#define PACMAN_SUBJECT_H
#include <unordered_set>
#include <algorithm>
#include "Observer.h"
/*
 *The Core idea is to creat a subscription method. the 2 main Classes in this relation are Subject and Observer:
 *  a subject keeps track of the observer(s)
 *  a subject notifies the observer(s) about the changes it's going through
 */

class Subject {
private:

  /*
   *unordered_set with raw pointers to observers in it.
   *Using unordered_set to prevent duplicate observer attachments
   *and provide O(1) detach performance
   */
  std::unordered_set<Observer*> observers;

public:

  //subscription methods:
  void attach(Observer* o) { // 1.External code connects observers
    observers.insert(o); // does nothing if already present
  }

  void detach(Observer* o) { // 2.External code disconnects observers
    observers.erase(o);
  }

  virtual ~Subject() = default; // Virtual destructor

protected:
  void notify(GameEvent event) {  // Derived classes call this when they change
    for (Observer* observer: observers) {
      observer->update(event);
    }
  }

};


#endif //PACMAN_SUBJECT_H