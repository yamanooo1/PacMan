#ifndef PACMAN_OBSERVER_H
#define PACMAN_OBSERVER_H

enum class GameEvent {
    COIN_COLLECTED,
    GHOST_EATEN,
    FRUIT_COLLECTED,
    PACMAN_DIED,
    LEVEL_CLEARED,
    DIRECTION_CHANGED,
    WALL_HIT
};

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(GameEvent event) = 0;
};

#endif