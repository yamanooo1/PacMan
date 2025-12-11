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
  SoundManager();

  std::map<SoundEffect, sf::SoundBuffer> soundBuffers;
  std::map<SoundEffect, std::vector<sf::Sound>> soundPlayers;

  sf::Music backgroundMusic;
  sf::Music menuMusic;
  sf::Music pauseScreenMusic;

  sf::Music movementSound;
  bool movementSoundPlaying;

  sf::Music fearModeSound;
  bool fearModeSoundPlaying;

  float soundVolume;
  float musicVolume;

  bool soundsLoaded;

  sf::Sound* getAvailableSound(SoundEffect effect);

public:
  static SoundManager& getInstance();

  SoundManager(const SoundManager&) = delete;
  SoundManager& operator=(const SoundManager&) = delete;

  bool loadSounds(const std::string& soundDirectory);

  void playSound(SoundEffect effect);

  void startMovementSound();
  void stopMovementSound();
  bool isMovementSoundPlaying() const { return movementSoundPlaying; }

  void startFearModeSound();
  void stopFearModeSound();
  bool isFearModeSoundPlaying() const { return fearModeSoundPlaying; }

  void playBackgroundMusic(bool loop = true);
  void playMenuMusic(bool loop = true);
  void playPauseMusic(bool loop = true);
  void stopMusic();
  void pauseMusic();
  void resumeMusic();

  void setSoundVolume(float volume);
  void setMusicVolume(float volume);
  float getSoundVolume() const { return soundVolume; }
  float getMusicVolume() const { return musicVolume; }

  void muteSounds();
  void unmuteSounds();
  void muteMusic();
  void unmuteMusic();

  bool isLoaded() const { return soundsLoaded; }
};

#endif