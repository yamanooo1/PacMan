//
// Exit.h - Exit gate entity ('w' in map)
//

#ifndef PACMAN_EXIT_H
#define PACMAN_EXIT_H

#include "EntityModel.h"

class Exit : public EntityModel {
public:
  Exit(float x, float y)
      : EntityModel(x, y, 1.0f, 1.0f) {}

  ~Exit() override = default;

  // Exits don't collide with anything - just mark positions
};

#endif // PACMAN_EXIT_H