//
// Observer.h - IMPROVED VERSION with additional events
//

#ifndef PACMAN_OBSERVER_H
#define PACMAN_OBSERVER_H

// ✅ ADD DIRECTION_CHANGED for Views to respond to movement
enum class GameEvent {
  COIN_COLLECTED,
  GHOST_EATEN,
  FRUIT_COLLECTED,
  PACMAN_DIED,
  LEVEL_CLEARED,
  DIRECTION_CHANGED,  // ← Triggered when PacMan/Ghost changes direction
  WALL_HIT           // ← NEW: Triggered when PacMan hits a wall
};

class Observer {
public:
  virtual ~Observer() = default;
  virtual void update(GameEvent event) = 0;
};

#endif //PACMAN_OBSERVER_H