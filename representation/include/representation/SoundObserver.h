//
// SoundObserver.h - Observes game events and plays sounds
// This belongs in REPRESENTATION, not logic!
//

#ifndef PACMAN_SOUNDOBSERVER_H
#define PACMAN_SOUNDOBSERVER_H

#include "logic/Observer.h"
#include "representation/SoundManager.h"
#include <memory>

// ✅ This class lives in REPRESENTATION layer
// It observes logic events and triggers sound effects
class SoundObserver : public Observer, public std::enable_shared_from_this<SoundObserver> {
public:
  SoundObserver();
  ~SoundObserver() override = default;

  // Observer interface - respond to game events
  void update(GameEvent event) override;
};

#endif // PACMAN_SOUNDOBSERVER_H