#include "logic/Score.h"
#include "logic/Stopwatch.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>

ScoreEntry::ScoreEntry(const std::string& name, int s)
    : playerName(name), score(s) {
}

Score::Score()
    : currentScore(0)
    , lastCoinTime(0.0f)
    , lastUpdateTime(0.0f)
    , pointsLostAccumulator(0.0f)
    , scoresFilePath("../../resources/data/highscores.txt") {
  Stopwatch& stopwatch = Stopwatch::getInstance();
  lastCoinTime = stopwatch.getElapsedTime();
  lastUpdateTime = stopwatch.getElapsedTime();

  loadHighScores();
}

void Score::update(GameEvent event) {
  Stopwatch& stopwatch = Stopwatch::getInstance();

  switch (event) {
    case GameEvent::COIN_COLLECTED: {
      float currentTime = stopwatch.getElapsedTime();
      float timeSinceLastCoin = currentTime - lastCoinTime;

      float decayFactor = std::pow(0.9f, timeSinceLastCoin / 0.5f);
      int points = static_cast<int>(30 * decayFactor);
      currentScore += points;

      lastCoinTime = currentTime;
      break;
    }

    case GameEvent::GHOST_EATEN:
      currentScore += 200;
      break;

    case GameEvent::FRUIT_COLLECTED:
      currentScore += 50;
      break;

    case GameEvent::LEVEL_CLEARED:
      currentScore += 1000;
      break;

    default:
      break;
  }
}

void Score::updateScoreDecay() {
  Stopwatch& stopwatch = Stopwatch::getInstance();
  float currentTime = stopwatch.getElapsedTime();
  float deltaTime = currentTime - lastUpdateTime;

  pointsLostAccumulator += 10.0f * deltaTime;

  int pointsToSubtract = static_cast<int>(pointsLostAccumulator);
  if (pointsToSubtract > 0) {
    currentScore -= pointsToSubtract;
    pointsLostAccumulator -= pointsToSubtract;

    if (currentScore < 0) {
      currentScore = 0;
      pointsLostAccumulator = 0.0f;
    }
  }

  lastUpdateTime = currentTime;
}

void Score::reset() {
  currentScore = 0;
  pointsLostAccumulator = 0.0f;
  Stopwatch& stopwatch = Stopwatch::getInstance();
  lastCoinTime = stopwatch.getElapsedTime();
  lastUpdateTime = stopwatch.getElapsedTime();
}

void Score::loadHighScores(const std::string& filepath) {
  scoresFilePath = filepath;
  highScores.clear();

  std::ifstream file(scoresFilePath);
  if (!file.is_open()) {
    return;
  }

  std::string line;
  while (std::getline(file, line) && highScores.size() < MAX_HIGH_SCORES) {
    std::istringstream iss(line);
    std::string name;
    int score;

    if (iss >> name >> score) {
      highScores.push_back(ScoreEntry(name, score));
    }
  }

  file.close();
}

void Score::saveHighScores() {
  std::ofstream file(scoresFilePath);
  if (!file.is_open()) {
    return;
  }

  for (const auto& entry : highScores) {
    file << entry.playerName << " " << entry.score << "\n";
  }

  file.close();
}

bool Score::isHighScore(int score) const {
  if (highScores.size() < MAX_HIGH_SCORES) {
    return true;
  }
  return score > highScores.back().score;
}

void Score::addHighScore(const std::string& playerName, int score) {
  highScores.push_back(ScoreEntry(playerName, score));

  std::sort(highScores.begin(), highScores.end(),
    [](const ScoreEntry& a, const ScoreEntry& b) {
      return a.score > b.score;
    });

  if (highScores.size() > MAX_HIGH_SCORES) {
    highScores.resize(MAX_HIGH_SCORES);
  }

  saveHighScores();
}