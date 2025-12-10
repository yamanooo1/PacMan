//
// SoundManager.h - Singleton manager for game audio
// UPDATED: Added pause music only (no wall hit sound)
//

#ifndef PACMAN_SOUNDMANAGER_H
#define PACMAN_SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

enum class SoundEffect {
  FRUIT_COLLECTED,
  GHOST_EATEN,
  PACMAN_DIED,
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
  sf::Music pauseScreenMusic;  // ✅ NEW: Pause screen music

  // ✅ Continuous movement sound (plays while PacMan moves)
  sf::Music movementSound;
  bool movementSoundPlaying;

  // ✅ Continuous fear mode sound (loops while fear mode active)
  sf::Music fearModeSound;
  bool fearModeSoundPlaying;

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

  // ✅ Movement sound controls
  void startMovementSound();
  void stopMovementSound();
  bool isMovementSoundPlaying() const { return movementSoundPlaying; }

  // ✅ Fear mode sound controls
  void startFearModeSound();
  void stopFearModeSound();
  bool isFearModeSoundPlaying() const { return fearModeSoundPlaying; }

  // Music controls
  void playBackgroundMusic(bool loop = true);
  void playMenuMusic(bool loop = true);
  void playPauseMusic(bool loop = true);  // ✅ NEW: Pause music
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