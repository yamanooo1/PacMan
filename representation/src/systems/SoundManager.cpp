#include "../../include/representation/systems/SoundManager.h"

namespace representation {

/**
 * @brief Initialize SoundManager with default settings
 *
 * Defaults:
 * - Sound volume: 50%
 * - Music volume: 30%
 * - All sounds: Not loaded
 */
SoundManager::SoundManager()
    : soundVolume(50.0f), musicVolume(30.0f), soundsLoaded(false), movementSoundPlaying(false),
      fearModeSoundPlaying(false) {}

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

void SoundManager::stopAllSounds() {
    for (auto& [effect, sounds] : soundPlayers) {
        for (auto& sound : sounds) {
            if (sound.getStatus() == sf::Sound::Playing) {
                sound.stop();
            }
        }
    }
}

/**
 * @brief Load all sound files from directory
 *
 * File structure:
 * soundDirectory/
 *   effects/
 *     11. PAC-MAN - Eating The Fruit.wav
 *     13. PAC-MAN - Eating The Ghost.wav
 *     15. Fail.wav
 *     03. PAC-MAN - Eating The Pac-dots.wav
 *     12. Ghost - Turn to Blue.wav
 *   Music/
 *     02. Start Music.wav
 *     meow.wav (pause music)
 *
 * Sound effect pooling:
 * - Creates 3 sf::Sound instances per effect
 * - All share same buffer (memory efficient)
 * - Allows overlapping plays
 */
bool SoundManager::loadSounds(const std::string& soundDirectory) {
    std::map<SoundEffect, std::string> soundFiles = {
        {SoundEffect::FRUIT_COLLECTED, soundDirectory + "/effects/11. PAC-MAN - Eating The Fruit.wav"},
        {SoundEffect::GHOST_EATEN, soundDirectory + "/effects/13. PAC-MAN - Eating The Ghost.wav"},
        {SoundEffect::PACMAN_DIED, soundDirectory + "/effects/15. Fail.wav"},
        {SoundEffect::LEVEL_START, soundDirectory + "/Music/02. Start Music.wav"}};

    int loadedCount = 0;
    for (const auto& [effect, filepath] : soundFiles) {
        sf::SoundBuffer buffer;

        if (buffer.loadFromFile(filepath)) {
            soundBuffers[effect] = buffer;

            // Create 3 sound instances for pooling
            soundPlayers[effect] = std::vector<sf::Sound>(3);
            for (auto& sound : soundPlayers[effect]) {
                sound.setBuffer(soundBuffers[effect]);
                sound.setVolume(soundVolume);
            }

            loadedCount++;
        }
    }

    // Load music tracks (streaming)
    if (backgroundMusic.openFromFile(soundDirectory + "/Music/02. Start Music.wav")) {
        backgroundMusic.setVolume(musicVolume);
    }

    if (pauseScreenMusic.openFromFile(soundDirectory + "/Music/meow.wav")) {
        pauseScreenMusic.setVolume(musicVolume);
    }

    // Load looping sounds (as Music for seamless loops)
    if (movementSound.openFromFile(soundDirectory + "/effects/03. PAC-MAN - Eating The Pac-dots.wav")) {
        movementSound.setVolume(soundVolume);
        movementSound.setLoop(true);
    }

    if (fearModeSound.openFromFile(soundDirectory + "/effects/12. Ghost - Turn to Blue.wav")) {
        fearModeSound.setVolume(musicVolume);
        fearModeSound.setLoop(true);
    }

    soundsLoaded = (loadedCount > 0);

    return soundsLoaded;
}

/**
 * @brief Find free sound slot or reuse oldest
 *
 * Priority:
 * 1. Find stopped sound
 * 2. If all playing, return first (oldest)
 */
sf::Sound* SoundManager::getAvailableSound(SoundEffect effect) {
    auto it = soundPlayers.find(effect);
    if (it == soundPlayers.end()) {
        return nullptr;
    }

    // Find first non-playing sound
    for (auto& sound : it->second) {
        if (sound.getStatus() != sf::Sound::Playing) {
            return &sound;
        }
    }

    // All playing, reuse first
    return &it->second[0];
}

/**
 * @brief Play one-shot sound effect
 *
 * Automatically manages pooling.
 */
void SoundManager::playSound(SoundEffect effect) {
    if (!soundsLoaded)
        return;

    sf::Sound* sound = getAvailableSound(effect);
    if (sound) {
        sound->play();
    }
}

/**
 * @brief Start waka-waka movement sound
 *
 * Only starts if not already playing (prevents restart stutter).
 */
void SoundManager::startMovementSound() {
    if (!soundsLoaded)
        return;

    if (movementSound.getStatus() != sf::Music::Playing) {
        movementSound.play();
        movementSoundPlaying = true;
    }
}

void SoundManager::stopMovementSound() {
    if (movementSound.getStatus() == sf::Music::Playing) {
        movementSound.stop();
        movementSoundPlaying = false;
    }
}

/**
 * @brief Start fear mode sound (ghost vulnerability)
 */
void SoundManager::startFearModeSound() {
    if (!soundsLoaded)
        return;

    if (fearModeSound.getStatus() != sf::Music::Playing) {
        fearModeSound.play();
        fearModeSoundPlaying = true;
    }
}

void SoundManager::stopFearModeSound() {
    if (fearModeSound.getStatus() == sf::Music::Playing) {
        fearModeSound.stop();
        fearModeSoundPlaying = false;
    }
}

/**
 * @brief Play background music (gameplay loop)
 */
void SoundManager::playBackgroundMusic(bool loop) {
    if (!soundsLoaded) {
        return;
    }

    if (backgroundMusic.getStatus() == sf::Music::Playing) {
        return;
    }

    backgroundMusic.setLoop(loop);
    backgroundMusic.play();
}

void SoundManager::playMenuMusic(bool loop) {
    if (menuMusic.getStatus() == sf::Music::Playing) {
        return;
    }

    menuMusic.setLoop(loop);
    menuMusic.play();
}

/**
 * @brief Play pause screen music (meow.wav)
 */
void SoundManager::playPauseMusic(bool loop) {
    if (pauseScreenMusic.getStatus() == sf::Music::Playing) {
        return;
    }

    pauseScreenMusic.setLoop(loop);
    pauseScreenMusic.play();
}

/**
 * @brief Stop all music and looping sounds
 *
 * Called on state transitions (menu, pause, level change).
 */
void SoundManager::stopMusic() {
    backgroundMusic.stop();
    menuMusic.stop();
    pauseScreenMusic.stop();
    stopMovementSound();
    stopFearModeSound();
}

/**
 * @brief Pause all active music
 *
 * Preserves playback position for resume.
 */
void SoundManager::pauseMusic() {
    backgroundMusic.pause();
    menuMusic.pause();
    pauseScreenMusic.pause();
    movementSound.pause();
    fearModeSound.pause();
}

/**
 * @brief Resume paused music
 *
 * Only resumes tracks that were playing before pause.
 */
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

/**
 * @brief Set sound effects volume
 *
 * Applies to:
 * - All sound effect instances
 * - Movement sound
 *
 * Clamped to 0-100 range.
 */
void SoundManager::setSoundVolume(float volume) {
    soundVolume = std::max(0.0f, std::min(100.0f, volume));

    for (auto& [effect, sounds] : soundPlayers) {
        for (auto& sound : sounds) {
            sound.setVolume(soundVolume);
        }
    }

    movementSound.setVolume(soundVolume);
}

/**
 * @brief Set music volume
 *
 * Applies to:
 * - Background music
 * - Menu music
 * - Pause music
 * - Fear mode sound
 *
 * Clamped to 0-100 range.
 */
void SoundManager::setMusicVolume(float volume) {
    musicVolume = std::max(0.0f, std::min(100.0f, volume));
    backgroundMusic.setVolume(musicVolume);
    menuMusic.setVolume(musicVolume);
    pauseScreenMusic.setVolume(musicVolume);
    fearModeSound.setVolume(musicVolume);
}

void SoundManager::muteSounds() { setSoundVolume(0.0f); }

void SoundManager::unmuteSounds() { setSoundVolume(50.0f); }

void SoundManager::muteMusic() { setMusicVolume(0.0f); }

void SoundManager::unmuteMusic() { setMusicVolume(30.0f); }

} // namespace representation