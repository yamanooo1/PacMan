//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_WORLD_H
#define PACMAN_WORLD_H
#include "EntityModel.h"
#include "AbstractFactory.h"
#include <memory>
#include <vector>

class World {
private:

  std::vector<std::unique_ptr<EntityModel>> entities;

  int mapWidth;
  int mapHeight;

  AbstractFactory* factory;

public:

  explicit World(AbstractFactory* f);

  void setMapDimensions(int w, int h);

  void addEntity(std::unique_ptr<EntityModel> entity);

  // Convenience methods - create entities using the factory
  void createPacMan(float x, float y);
  void createGhost(float x, float y);
  void createWall(float x, float y, float w, float h);
  void createCoin(float x, float y);
  void createFruit(float x, float y);


  bool loadFromFile(const std::string& filename);

  int getMapWidth() const { return mapWidth; }
  int getMapHeight() const { return mapHeight; }
  const std::vector<std::unique_ptr<EntityModel>>& getEntities() const { return entities; }
  size_t getEntityCount() const { return entities.size(); }
};


#endif //PACMAN_WORLD_H