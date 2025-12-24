#include "../../include/representation/systems/SoundManager.h"

namespace representation {

SoundManager::SoundManager()
    : soundVolume(50.0f), musicVolume(30.0f), soundsLoaded(false), movementSoundPlaying(false),
      fearModeSoundPlaying(false) {}

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

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

            soundPlayers[effect] = std::vector<sf::Sound>(3);
            for (auto& sound : soundPlayers[effect]) {
                sound.setBuffer(soundBuffers[effect]);
                sound.setVolume(soundVolume);
            }

            loadedCount++;
        }
    }

    if (backgroundMusic.openFromFile(soundDirectory + "/Music/02. Start Music.wav")) {
        backgroundMusic.setVolume(musicVolume);
    }

    if (pauseScreenMusic.openFromFile(soundDirectory + "/Music/meow.wav")) {
        pauseScreenMusic.setVolume(musicVolume);
    }

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

sf::Sound* SoundManager::getAvailableSound(SoundEffect effect) {
    auto it = soundPlayers.find(effect);
    if (it == soundPlayers.end()) {
        return nullptr;
    }

    for (auto& sound : it->second) {
        if (sound.getStatus() != sf::Sound::Playing) {
            return &sound;
        }
    }

    return &it->second[0];
}

void SoundManager::playSound(SoundEffect effect) {
    if (!soundsLoaded)
        return;

    sf::Sound* sound = getAvailableSound(effect);
    if (sound) {
        sound->play();
    }
}

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

void SoundManager::playPauseMusic(bool loop) {
    if (pauseScreenMusic.getStatus() == sf::Music::Playing) {
        return;
    }

    pauseScreenMusic.setLoop(loop);
    pauseScreenMusic.play();
}

void SoundManager::stopMusic() {
    backgroundMusic.stop();
    menuMusic.stop();
    pauseScreenMusic.stop();
    stopMovementSound();
    stopFearModeSound();
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

    for (auto& [effect, sounds] : soundPlayers) {
        for (auto& sound : sounds) {
            sound.setVolume(soundVolume);
        }
    }

    movementSound.setVolume(soundVolume);
}

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