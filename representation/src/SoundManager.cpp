//
// SoundManager.cpp - Implementation of audio system
//

#include "representation/SoundManager.h"
#include <iostream>

SoundManager::SoundManager()
    : soundVolume(50.0f)
    , musicVolume(30.0f)
    , soundsLoaded(false)
{
  std::cout << "[SOUND] SoundManager initialized" << std::endl;
}

SoundManager& SoundManager::getInstance() {
  static SoundManager instance;
  return instance;
}

bool SoundManager::loadSounds(const std::string& soundDirectory) {
  std::cout << "[SOUND] Loading sounds from: " << soundDirectory << std::endl;

  // Map sound effects to their file names
  std::map<SoundEffect, std::string> soundFiles = {
    {SoundEffect::COIN_COLLECTED, soundDirectory + "/03. PAC-MAN - Eating The Pac-dots.mp3"},
    {SoundEffect::FRUIT_COLLECTED, soundDirectory + "/11. PAC-MAN - Eating The Fruit.mp3"},
    {SoundEffect::GHOST_EATEN, soundDirectory + "/13. PAC-MAN - Eating The Ghost.mp3"},
    {SoundEffect::PACMAN_DIED, soundDirectory + "/15. Fail.mp3"},
    {SoundEffect::FEAR_MODE_START, soundDirectory + "/12. Ghost - Turn to Blue.mp3"},
    {SoundEffect::LEVEL_START, soundDirectory + "/02. Start Music.mp3"}
  };

  // Load each sound buffer
  int loadedCount = 0;
  for (const auto& [effect, filepath] : soundFiles) {
    sf::SoundBuffer buffer;

    if (buffer.loadFromFile(filepath)) {
      soundBuffers[effect] = buffer;

      // Create 3 sound players for this effect (allows overlapping)
      soundPlayers[effect] = std::vector<sf::Sound>(3);
      for (auto& sound : soundPlayers[effect]) {
        sound.setBuffer(soundBuffers[effect]);
        sound.setVolume(soundVolume);
      }

      loadedCount++;
      std::cout << "[SOUND] ✓ Loaded: " << filepath << std::endl;
    } else {
      std::cerr << "[SOUND] ✗ Failed to load: " << filepath << std::endl;
    }
  }

  // Load background music
  if (backgroundMusic.openFromFile(soundDirectory + "/02. Start Music.mp3")) {
    backgroundMusic.setVolume(musicVolume);
    std::cout << "[SOUND] ✓ Loaded background music" << std::endl;
  } else {
    std::cerr << "[SOUND] ✗ Failed to load background music" << std::endl;
  }

  // Load menu music
  if (menuMusic.openFromFile(soundDirectory + "/16. Coffee Break Music.mp3")) {
    menuMusic.setVolume(musicVolume);
    std::cout << "[SOUND] ✓ Loaded menu music" << std::endl;
  } else {
    std::cerr << "[SOUND] ✗ Failed to load menu music" << std::endl;
  }

  soundsLoaded = (loadedCount > 0);
  std::cout << "[SOUND] Loaded " << loadedCount << "/" << soundFiles.size()
            << " sound effects" << std::endl;

  return soundsLoaded;
}

sf::Sound* SoundManager::getAvailableSound(SoundEffect effect) {
  auto it = soundPlayers.find(effect);
  if (it == soundPlayers.end()) {
    return nullptr;
  }

  // Find a sound that's not currently playing
  for (auto& sound : it->second) {
    if (sound.getStatus() != sf::Sound::Playing) {
      return &sound;
    }
  }

  // If all are playing, use the first one (restart it)
  return &it->second[0];
}

void SoundManager::playSound(SoundEffect effect) {
  if (!soundsLoaded) return;

  sf::Sound* sound = getAvailableSound(effect);
  if (sound) {
    sound->play();

    // Debug output
    const char* effectName = "UNKNOWN";
    switch (effect) {
      case SoundEffect::COIN_COLLECTED: effectName = "COIN"; break;
      case SoundEffect::FRUIT_COLLECTED: effectName = "FRUIT"; break;
      case SoundEffect::GHOST_EATEN: effectName = "GHOST_EATEN"; break;
      case SoundEffect::PACMAN_DIED: effectName = "DEATH"; break;
      case SoundEffect::FEAR_MODE_START: effectName = "FEAR_MODE"; break;
      case SoundEffect::LEVEL_START: effectName = "LEVEL_START"; break;
    }

    std::cout << "[SOUND] Playing: " << effectName << std::endl;
  }
}

void SoundManager::playBackgroundMusic(bool loop) {
  if (backgroundMusic.getStatus() == sf::Music::Playing) {
    return;  // Already playing
  }

  backgroundMusic.setLoop(loop);
  backgroundMusic.play();
  std::cout << "[SOUND] Started background music (loop: " << loop << ")" << std::endl;
}

void SoundManager::playMenuMusic(bool loop) {
  if (menuMusic.getStatus() == sf::Music::Playing) {
    return;  // Already playing
  }

  menuMusic.setLoop(loop);
  menuMusic.play();
  std::cout << "[SOUND] Started menu music (loop: " << loop << ")" << std::endl;
}

void SoundManager::stopMusic() {
  backgroundMusic.stop();
  menuMusic.stop();
  std::cout << "[SOUND] Stopped all music" << std::endl;
}

void SoundManager::pauseMusic() {
  backgroundMusic.pause();
  menuMusic.pause();
}

void SoundManager::resumeMusic() {
  if (backgroundMusic.getStatus() == sf::Music::Paused) {
    backgroundMusic.play();
  }
  if (menuMusic.getStatus() == sf::Music::Paused) {
    menuMusic.play();
  }
}

void SoundManager::setSoundVolume(float volume) {
  soundVolume = std::max(0.0f, std::min(100.0f, volume));

  // Update all sound players
  for (auto& [effect, sounds] : soundPlayers) {
    for (auto& sound : sounds) {
      sound.setVolume(soundVolume);
    }
  }

  std::cout << "[SOUND] Sound volume set to: " << soundVolume << std::endl;
}

void SoundManager::setMusicVolume(float volume) {
  musicVolume = std::max(0.0f, std::min(100.0f, volume));
  backgroundMusic.setVolume(musicVolume);
  menuMusic.setVolume(musicVolume);

  std::cout << "[SOUND] Music volume set to: " << musicVolume << std::endl;
}

void SoundManager::muteSounds() {
  setSoundVolume(0.0f);
}

void SoundManager::unmuteSounds() {
  setSoundVolume(50.0f);
}

void SoundManager::muteMusic() {
  setMusicVolume(0.0f);
}

void SoundManager::unmuteMusic() {
  setMusicVolume(30.0f);
}