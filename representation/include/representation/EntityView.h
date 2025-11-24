//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_ENTITYVIEW_H
#define PACMAN_ENTITYVIEW_H
#include "logic/EntityModel.h"
#include "logic/Observer.h"

class EntityView: public Observer {
private:
public:
  //todo virtual draw();
protected:
  EntityModel* model;
};


#endif //PACMAN_ENTITYVIEW_H