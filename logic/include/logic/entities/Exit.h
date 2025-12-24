#ifndef PACMAN_EXIT_H
#define PACMAN_EXIT_H
#include "../core/EntityModel.h"

namespace logic {

class Exit : public EntityModel {
public:
    Exit(float x, float y);
    ~Exit() override = default;
};

} // namespace logic

#endif