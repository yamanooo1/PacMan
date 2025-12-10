#ifndef PACMAN_EXIT_H
#define PACMAN_EXIT_H
#include "EntityModel.h"

class Exit : public EntityModel {
public:
  Exit(float x, float y);
  ~Exit() override = default;
};

#endif