#ifndef PACMAN_SOUNDOBSERVER_H
#define PACMAN_SOUNDOBSERVER_H

#include "../../../../logic/include/logic/patterns/Observer.h"
#include "SoundManager.h"
#include <memory>

namespace representation {

class SoundObserver : public logic::Observer, public std::enable_shared_from_this<SoundObserver> {
public:
    SoundObserver();
    ~SoundObserver() override = default;

    void update(logic::GameEvent event) override;
};

} // namespace representation

#endif