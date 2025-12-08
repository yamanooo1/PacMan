#ifndef PACMAN_WORLD_H
#define PACMAN_WORLD_H

#include "EntityModel.h"
#include "AbstractFactory.h"
#include <memory>
#include <vector>

class PacMan;
class Ghost;
enum class GhostType;
class Wall;
class Coin;
class Fruit;
class Score;
class Lives;

class World {
private:
  std::vector<std::unique_ptr<EntityModel>> entities;
  std::vector<Ghost*> ghosts;  // ⚠️ Non-owning - point to entities owned above

  int mapWidth;
  int mapHeight;

  int currentLevel;
  float ghostSpeedMultiplier;
  float fearDurationMultiplier;

  AbstractFactory* factory;  // ⚠️ Non-owning - owned by LevelState
  PacMan* pacman;  // ⚠️ Non-owning - points to entity in 'entities' vector

  // ✅ CHANGED to shared_ptr (for Observer pattern)
  std::shared_ptr<Score> score;
  std::shared_ptr<Lives> lives;

  float pacmanSpawnX, pacmanSpawnY;
  std::vector<std::pair<float, float>> ghostSpawnPositions;

  bool fearModeActive;
  float fearModeTimer;
  float fearModeDuration;

  bool deathAnimationActive;
  float deathAnimationTimer;
  float deathAnimationDuration;

  bool readyStateActive;
  float readyStateTimer;
  float readyStateDuration;

  bool levelCleared;
  bool levelClearedDisplayActive;
  float levelClearedDisplayTimer;
  float levelClearedDisplayDuration;

public:
  explicit World(AbstractFactory* f, int level = 1);

  void setMapDimensions(int w, int h);

  // ✅ CHANGED to take shared_ptr
  void setScore(std::shared_ptr<Score> s) { score = s; }
  void setLives(std::shared_ptr<Lives> l) { lives = l; }

  bool isLevelCleared() const { return levelCleared; }
  bool isLevelClearedDisplayActive() const { return levelClearedDisplayActive; }

  void addEntity(std::unique_ptr<EntityModel> entity);

  void createPacMan(float x, float y);
  void createGhost(float x, float y, GhostType type, GhostColor color, float waitTime);
  void createWall(float x, float y, float w, float h);
  void createCoin(float x, float y);
  void createFruit(float x, float y);

  bool loadFromFile(const std::string& filename);
  bool isFearModeActive() const { return fearModeActive; }

  int getMapWidth() const { return mapWidth; }
  int getMapHeight() const { return mapHeight; }
  const std::vector<std::unique_ptr<EntityModel>>& getEntities() const { return entities; }
  size_t getEntityCount() const { return entities.size(); }

  void update(float deltaTime);
  PacMan* getPacMan() const { return pacman; }

  void respawnPacManAndGhosts();

  bool isWall(float x, float y, float width, float height) const;
  bool canMoveInDirection(float x, float y, float w, float h, Direction dir, float distance) const;
  bool hasWallInGridCell(int gridX, int gridY) const;

  void activateFearMode();
  float getGhostSpeedMultiplier() const { return ghostSpeedMultiplier; }

  void updateFearMode(float deltaTime);
  bool isFearModeEnding() const { return fearModeActive && fearModeTimer <= 2.0f; }

  void startDeathAnimation();
  void updateDeathAnimation(float deltaTime);
  bool isDeathAnimationActive() const { return deathAnimationActive; }

  void startReadyState();
  void updateReadyState(float deltaTime);
  bool isReadyStateActive() const { return readyStateActive; }

  bool isExitPosition(int gridX, int gridY) const;
  std::vector<std::pair<int, int>> getExitPositions() const;

private:
  void updatePacMan(float deltaTime);
  void updateGhosts(float deltaTime);
  void checkCollisions();
  void removeDeadEntities();
  void checkLevelComplete();

  std::vector<std::pair<int, int>> exitPositions;
};

#endif //PACMAN_WORLD_H