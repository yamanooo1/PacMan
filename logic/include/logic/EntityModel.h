//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_ENTITY_H
#define PACMAN_ENTITY_H
#include "Subject.h"

enum class Direction {
  UP, Down, Left, Right, None
};
class EntityModel: public Subject {

private:
  // x,y
  std::pair<float,float> cords;
  //width, height
  std::pair<float,float> HitBox;

  float speed;
  Direction dir;

public:
};


#endif //PACMAN_ENTITY_H