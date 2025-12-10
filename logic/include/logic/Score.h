#ifndef PACMAN_SCORE_H
#define PACMAN_SCORE_H
#include "Observer.h"
#include <vector>
#include <string>
#include <memory>

struct ScoreEntry {
  std::string playerName;
  int score;

  ScoreEntry(const std::string& name = "PLAYER", int s = 0);
};

class Score : public Observer, public std::enable_shared_from_this<Score> {
private:
  int currentScore;
  float lastCoinTime;
  float lastUpdateTime;
  float pointsLostAccumulator;

  static const int MAX_HIGH_SCORES = 5;
  std::vector<ScoreEntry> highScores;
  std::string scoresFilePath;

public:
  Score();
  ~Score() override = default;

  void update(GameEvent event) override;
  void updateScoreDecay();

  int getScore() const { return currentScore; }
  void reset();

  void loadHighScores(const std::string& filepath = "../../resources/data/highscores.txt");
  void saveHighScores();
  bool isHighScore(int score) const;
  void addHighScore(const std::string& playerName, int score);
  std::vector<ScoreEntry> getHighScores() const { return highScores; }
  void setScore(int newScore) { currentScore = newScore; }
};

#endif