#ifndef PACMAN_SCORE_H
#define PACMAN_SCORE_H

#include "Observer.h"
#include <vector>
#include <string>

class Score : public Observer {
private:
  int currentScore = 0;
  float lastCoinTime = 0.0f;
  float lastUpdateTime = 0.0f;
  float pointsLostAccumulator = 0.0f;

  // ✅ NEW: High scores management
  static const int MAX_HIGH_SCORES = 5;
  std::vector<int> highScores;
  std::string scoresFilePath;

public:
  Score();
  ~Score() override = default;

  void update(GameEvent event) override;
  void updateScoreDecay();

  int getScore() const { return currentScore; }
  void reset();

  // ✅ NEW: High scores methods
  void loadHighScores(const std::string& filepath = "../../resources/data/highscores.txt");
  void saveHighScores();
  bool isHighScore(int score) const;
  void addHighScore(int score);
  std::vector<int> getHighScores() const { return highScores; }
};

#endif // PACMAN_SCORE_H