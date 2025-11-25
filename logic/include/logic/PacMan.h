//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H
#include "EntityModel.h"

class PacMan: public EntityModel {
public:
  PacMan(float x, float y) : EntityModel(x, y, 0.05f, 0.05f) {};

  ~PacMan() override = default;
};


#endif //PACMAN_PACMAN_H