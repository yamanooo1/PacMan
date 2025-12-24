#ifndef PACMAN_WALL_H
#define PACMAN_WALL_H
#include "../core/EntityModel.h"

namespace logic {

class Wall : public EntityModel {
public:
    Wall(float x, float y, float width, float height);
    ~Wall() override = default;
};

} // namespace logic

#endif