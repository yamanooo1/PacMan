#ifndef PACMAN_WORLD_H
#define PACMAN_WORLD_H
#include "AbstractFactory.h"
#include "EntityModel.h"
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
  std::vector<Ghost*> ghosts;

  int mapWidth;
  int mapHeight;
  int currentLevel;
  float ghostSpeedMultiplier;
  float fearDurationMultiplier;

  AbstractFactory* factory;
  PacMan* pacman;

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

  std::vector<std::pair<int, int>> exitPositions;

  void updatePacMan(float deltaTime) const;
  void updateGhosts(float deltaTime);
  void checkCollisions();
  void removeDeadEntities();
  void checkLevelComplete();

public:
  explicit World(AbstractFactory* f, int level = 1);

  void setMapDimensions(int w, int h);
  void setScore(const std::shared_ptr<Score>& s) { score = s; }
  void setLives(const std::shared_ptr<Lives>& l) { lives = l; }

  [[nodiscard]] bool isLevelCleared() const { return levelCleared; }
  [[nodiscard]] bool isLevelClearedDisplayActive() const { return levelClearedDisplayActive; }

  void addEntity(std::unique_ptr<EntityModel> entity);

  void createPacMan(float x, float y);
  void createGhost(float x, float y, GhostType type, GhostColor color, float waitTime);
  void createWall(float x, float y, float w, float h);
  void createCoin(float x, float y);
  void createFruit(float x, float y);

  bool loadFromFile(const std::string& filename);
  [[nodiscard]] bool isFearModeActive() const { return fearModeActive; }

  [[nodiscard]] int getMapWidth() const { return mapWidth; }
  [[nodiscard]] int getMapHeight() const { return mapHeight; }
  [[nodiscard]] const std::vector<std::unique_ptr<EntityModel>>& getEntities() const { return entities; }
  [[nodiscard]] size_t getEntityCount() const { return entities.size(); }

  void update(float deltaTime);
  [[nodiscard]] PacMan* getPacMan() const { return pacman; }

  void respawnPacManAndGhosts();

  [[nodiscard]] bool isWall(float x, float y, float width, float height) const;
  [[nodiscard]] bool hasWallInGridCell(int gridX, int gridY) const;

  void activateFearMode();
  [[nodiscard]] float getGhostSpeedMultiplier() const { return ghostSpeedMultiplier; }

  void updateFearMode(float deltaTime);
  [[nodiscard]] bool isFearModeEnding() const;

  void startDeathAnimation();
  void updateDeathAnimation(float deltaTime);
  [[nodiscard]] bool isDeathAnimationActive() const { return deathAnimationActive; }

  void startReadyState();
  void updateReadyState(float deltaTime);
  [[nodiscard]] bool isReadyStateActive() const { return readyStateActive; }

  [[nodiscard]] bool isExitPosition(int gridX, int gridY) const;
  [[nodiscard]] std::vector<std::pair<int, int>> getExitPositions() const;
};

#endif