#ifndef PACMAN_SOUNDOBSERVER_H
#define PACMAN_SOUNDOBSERVER_H

#include "logic/Observer.h"
#include "representation/SoundManager.h"
#include <memory>

class SoundObserver : public Observer, public std::enable_shared_from_this<SoundObserver> {
public:
  SoundObserver();
  ~SoundObserver() override = default;

  void update(GameEvent event) override;
};

#endif