#ifndef PACMAN_COIN_H
#define PACMAN_COIN_H
#include "Collectable.h"

namespace logic {

class Coin : public Collectable {
public:
    Coin(float x, float y);
    ~Coin() override = default;

    void onCollected() override;
};

} // namespace logic

#endif