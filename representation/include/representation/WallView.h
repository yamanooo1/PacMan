//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_WALLVIEW_H
#define PACMAN_WALLVIEW_H
#include "EntityView.h"

class WallView: public EntityView {
public:
  void update(GameEvent) override;
};


#endif //PACMAN_WALLVIEW_H