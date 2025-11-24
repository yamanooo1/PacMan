//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_SCORE_H
#define PACMAN_SCORE_H
#include "Observer.h"

class Score: public Observer{
private:
  int currentScore;
public:
  //todo
  void update() override;

  int getScore() const { return currentScore; }
};


#endif //PACMAN_SCORE_H