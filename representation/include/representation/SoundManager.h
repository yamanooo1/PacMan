//
// SoundManager.h - Singleton manager for game audio
//

#ifndef PACMAN_SOUNDMANAGER_H
#define PACMAN_SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

enum class SoundEffect {
  COIN_COLLECTED,
  FRUIT_COLLECTED,
  GHOST_EATEN,
  PACMAN_DIED,
  FEAR_MODE_START,
  LEVEL_START
};

class SoundManager {
private:
  // Singleton instance
  SoundManager();

  // Sound buffers (hold audio data)
  std::map<SoundEffect, sf::SoundBuffer> soundBuffers;

  // Sound players (multiple for overlapping sounds)
  std::map<SoundEffect, std::vector<sf::Sound>> soundPlayers;

  // Background music
  sf::Music backgroundMusic;
  sf::Music menuMusic;

  // Volume settings (0-100)
  float soundVolume;
  float musicVolume;

  bool soundsLoaded;

  // Helper: Find available sound player for effect
  sf::Sound* getAvailableSound(SoundEffect effect);

public:
  // Singleton access
  static SoundManager& getInstance();

  // Delete copy/move
  SoundManager(const SoundManager&) = delete;
  SoundManager& operator=(const SoundManager&) = delete;

  // Load all sound effects from directory
  bool loadSounds(const std::string& soundDirectory);

  // Play sound effects
  void playSound(SoundEffect effect);

  // Music controls
  void playBackgroundMusic(bool loop = true);
  void playMenuMusic(bool loop = true);
  void stopMusic();
  void pauseMusic();
  void resumeMusic();

  // Volume controls
  void setSoundVolume(float volume);  // 0-100
  void setMusicVolume(float volume);  // 0-100
  float getSoundVolume() const { return soundVolume; }
  float getMusicVolume() const { return musicVolume; }

  // Mute/unmute
  void muteSounds();
  void unmuteSounds();
  void muteMusic();
  void unmuteMusic();

  // Check if sounds loaded successfully
  bool isLoaded() const { return soundsLoaded; }
};

#endif // PACMAN_SOUNDMANAGER_H