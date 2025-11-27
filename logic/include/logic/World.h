#ifndef PACMAN_WORLD_H
#define PACMAN_WORLD_H
#include "EntityModel.h"
#include "AbstractFactory.h"
#include <memory>
#include <vector>

class PacMan;
class Ghost;
class Wall;
class Coin;
class Fruit;
class Score;
class Lives;

class World {
private:
  std::vector<std::unique_ptr<EntityModel>> entities;

  int mapWidth;
  int mapHeight;

  AbstractFactory* factory;
  PacMan* pacman;
  Score* score;
  Lives* lives;  // NEW: Lives system

  // Spawn positions for reset
  float pacmanSpawnX, pacmanSpawnY;
  std::vector<std::pair<float, float>> ghostSpawnPositions;  // NEW: Track ghost spawns

public:
  explicit World(AbstractFactory* f);

  void setMapDimensions(int w, int h);
  void setScore(Score* s) { score = s; }
  void setLives(Lives* l) { lives = l; }  // NEW: Connect Lives

  void addEntity(std::unique_ptr<EntityModel> entity);

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

  void update(float deltaTime);
  PacMan* getPacMan() const { return pacman; }

  // NEW: Respawn methods
  void respawnPacManAndGhosts();

  // Collision helpers
  bool isWall(float x, float y, float width, float height) const;
  bool canMoveInDirection(float x, float y, float w, float h, Direction dir, float distance) const;

private:
  void updatePacMan(float deltaTime);
  void updateGhosts(float deltaTime);
  void checkCollisions();
  void removeDeadEntities();

  // Helper to check if a specific grid cell contains a wall
  bool hasWallInGridCell(int gridX, int gridY) const;
};

#endif //PACMAN_WORLD_H