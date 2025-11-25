//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_WALL_H
#define PACMAN_WALL_H
#include "EntityModel.h"

class Wall: public EntityModel {
public:
  Wall(float x, float y, float width, float height) : EntityModel(x, y, width, height) {};

  ~Wall() override = default;
};


#endif //PACMAN_WALL_H