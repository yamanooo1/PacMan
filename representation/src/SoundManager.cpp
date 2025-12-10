//
// SoundManager.cpp - Implementation of audio system
// UPDATED: Added pause music only (no wall hit sound)
//

#include "representation/SoundManager.h"
#include <iostream>

SoundManager::SoundManager()
    : soundVolume(50.0f)
    , musicVolume(30.0f)
    , soundsLoaded(false)
    , movementSoundPlaying(false)
    , fearModeSoundPlaying(false)
{
  std::cout << "[SOUND] SoundManager initialized" << std::endl;
}

SoundManager& SoundManager::getInstance() {
  static SoundManager instance;
  return instance;
}

bool SoundManager::loadSounds(const std::string& soundDirectory) {
  std::cout << "[SOUND] Loading sounds from: " << soundDirectory << std::endl;

  std::map<SoundEffect, std::string> soundFiles = {
    {SoundEffect::FRUIT_COLLECTED, soundDirectory + "/11. PAC-MAN - Eating The Fruit.wav"},
    {SoundEffect::GHOST_EATEN, soundDirectory + "/13. PAC-MAN - Eating The Ghost.wav"},
    {SoundEffect::PACMAN_DIED, soundDirectory + "/15. Fail.wav"},
    {SoundEffect::LEVEL_START, soundDirectory + "/02. Start Music.wav"}
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
  if (backgroundMusic.openFromFile(soundDirectory + "/02. Start Music.wav")) {
    backgroundMusic.setVolume(musicVolume);
    std::cout << "[SOUND] ✓ Loaded background music" << std::endl;
  } else {
    std::cerr << "[SOUND] ✗ Failed to load background music" << std::endl;
  }

  // ✅ NEW: Load pause screen music (same as menu music)
  if (pauseScreenMusic.openFromFile(soundDirectory + "/meow.wav")) {
    pauseScreenMusic.setVolume(musicVolume);
    std::cout << "[SOUND] ✓ Loaded pause screen music" << std::endl;
  } else {
    std::cerr << "[SOUND] ✗ Failed to load pause screen music" << std::endl;
  }

  // Load continuous movement sound
  if (movementSound.openFromFile(soundDirectory + "/03. PAC-MAN - Eating The Pac-dots.wav")) {
    movementSound.setVolume(soundVolume);
    movementSound.setLoop(true);  // Loop continuously
    std::cout << "[SOUND] ✓ Loaded movement sound (will loop)" << std::endl;
  } else {
    std::cerr << "[SOUND] ✗ Failed to load movement sound" << std::endl;
  }

  // Load continuous fear mode sound
  if (fearModeSound.openFromFile(soundDirectory + "/12. Ghost - Turn to Blue.wav")) {
    fearModeSound.setVolume(musicVolume);  // Use music volume
    fearModeSound.setLoop(true);  // Loop continuously while fear mode active
    std::cout << "[SOUND] ✓ Loaded fear mode sound (will loop)" << std::endl;
  } else {
    std::cerr << "[SOUND] ✗ Failed to load fear mode sound" << std::endl;
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
      case SoundEffect::FRUIT_COLLECTED: effectName = "FRUIT"; break;
      case SoundEffect::GHOST_EATEN: effectName = "GHOST_EATEN"; break;
      case SoundEffect::PACMAN_DIED: effectName = "DEATH"; break;
      case SoundEffect::LEVEL_START: effectName = "LEVEL_START"; break;
    }

    std::cout << "[SOUND] Playing: " << effectName << std::endl;
  }
}

// Start continuous movement sound
void SoundManager::startMovementSound() {
  if (!soundsLoaded) return;

  // Only start if not already playing
  if (movementSound.getStatus() != sf::Music::Playing) {
    movementSound.play();
    movementSoundPlaying = true;
    std::cout << "[SOUND] Started movement sound (looping)" << std::endl;
  }
}

// Stop continuous movement sound
void SoundManager::stopMovementSound() {
  if (movementSound.getStatus() == sf::Music::Playing) {
    movementSound.stop();
    movementSoundPlaying = false;
    std::cout << "[SOUND] Stopped movement sound" << std::endl;
  }
}

// Start continuous fear mode sound
void SoundManager::startFearModeSound() {
  if (!soundsLoaded) return;

  // Only start if not already playing
  if (fearModeSound.getStatus() != sf::Music::Playing) {
    fearModeSound.play();
    fearModeSoundPlaying = true;
    std::cout << "[SOUND] Started fear mode sound (looping)" << std::endl;
  }
}

// Stop continuous fear mode sound
void SoundManager::stopFearModeSound() {
  if (fearModeSound.getStatus() == sf::Music::Playing) {
    fearModeSound.stop();
    fearModeSoundPlaying = false;
    std::cout << "[SOUND] Stopped fear mode sound" << std::endl;
  }
}

void SoundManager::playBackgroundMusic(bool loop) {
  if (!soundsLoaded) {
    std::cout << "[SOUND] Cannot play background music - sounds not loaded yet" << std::endl;
    return;
  }

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

// ✅ NEW: Play pause screen music
void SoundManager::playPauseMusic(bool loop) {
  if (pauseScreenMusic.getStatus() == sf::Music::Playing) {
    return;  // Already playing
  }

  pauseScreenMusic.setLoop(loop);
  pauseScreenMusic.play();
  std::cout << "[SOUND] Started pause screen music (loop: " << loop << ")" << std::endl;
}

void SoundManager::stopMusic() {
  backgroundMusic.stop();
  menuMusic.stop();
  pauseScreenMusic.stop();
  stopMovementSound();
  stopFearModeSound();
  std::cout << "[SOUND] Stopped all music" << std::endl;
}

void SoundManager::pauseMusic() {
  backgroundMusic.pause();
  menuMusic.pause();
  pauseScreenMusic.pause();
  movementSound.pause();
  fearModeSound.pause();
}

void SoundManager::resumeMusic() {
  if (backgroundMusic.getStatus() == sf::Music::Paused) {
    backgroundMusic.play();
  }
  if (menuMusic.getStatus() == sf::Music::Paused) {
    menuMusic.play();
  }
  if (pauseScreenMusic.getStatus() == sf::Music::Paused) {
    pauseScreenMusic.play();
  }
  if (movementSoundPlaying && movementSound.getStatus() == sf::Music::Paused) {
    movementSound.play();
  }
  if (fearModeSoundPlaying && fearModeSound.getStatus() == sf::Music::Paused) {
    fearModeSound.play();
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

  // Also update movement sound volume
  movementSound.setVolume(soundVolume);

  std::cout << "[SOUND] Sound volume set to: " << soundVolume << std::endl;
}

void SoundManager::setMusicVolume(float volume) {
  musicVolume = std::max(0.0f, std::min(100.0f, volume));
  backgroundMusic.setVolume(musicVolume);
  menuMusic.setVolume(musicVolume);
  pauseScreenMusic.setVolume(musicVolume);
  fearModeSound.setVolume(musicVolume);

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