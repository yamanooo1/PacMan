#ifndef PACMAN_FRUIT_H
#define PACMAN_FRUIT_H
#include "Collectable.h"

namespace logic {

class Fruit : public Collectable {
public:
    Fruit(float x, float y);
    ~Fruit() override = default;

    void onCollected() override;
};

} // namespace logic

#endif