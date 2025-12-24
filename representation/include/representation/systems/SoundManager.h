#ifndef PACMAN_SOUNDMANAGER_H
#define PACMAN_SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <map>
#include <string>

namespace representation {

/**
 * @brief Sound effect types
 */
enum class SoundEffect { FRUIT_COLLECTED, GHOST_EATEN, PACMAN_DIED, LEVEL_START };

/**
 * @brief SoundManager - singleton audio system
 *
 * DESIGN PATTERN: Singleton (global audio control)
 *
 * Purpose:
 * - Centralized audio management
 * - Play sound effects (fruit, ghost, death)
 * - Control music (background, menu, pause)
 * - Manage looping sounds (movement, fear mode)
 * - Volume control (separate for SFX and music)
 *
 * Audio Architecture:
 * ```
 * Sound Effects (one-shot):
 *   sf::SoundBuffer (loaded once)
 *     → sf::Sound[3] (3 instances per effect)
 *     → Play first available, or override oldest
 *
 * Music Tracks (streaming):
 *   sf::Music (streams from file)
 *     → backgroundMusic (gameplay)
 *     → menuMusic (main menu)
 *     → pauseScreenMusic (pause overlay)
 *
 * Looping Sounds (pseudo-music):
 *   sf::Music (for seamless looping)
 *     → movementSound (waka-waka)
 *     → fearModeSound (vulnerable ghosts)
 * ```
 *
 * Sound Effect Pooling:
 * - Each effect has 3 sf::Sound instances
 * - Allows overlapping sounds (e.g., 3 ghosts eaten simultaneously)
 * - getAvailableSound() finds free slot or reuses
 *
 * Looping Sound Management:
 * - movementSound/fearModeSound use sf::Music for seamless loops
 * - Separate tracking flags (movementSoundPlaying, fearModeSoundPlaying)
 * - Start/stop methods check flags to prevent restart
 * - Pause/resume maintains playing state
 *
 * Volume Levels:
 * - Sound effects: 50% default (coins, ghosts, etc.)
 * - Music: 30% default (background tracks)
 * - Independent control (setSoundVolume, setMusicVolume)
 *
 * File Paths:
 * - Base directory: "../../resources/Sounds/"
 * - Effects: "/effects/*.wav"
 * - Music: "/Music/*.wav"
 *
 * State Transitions:
 * - stopMusic(): Stops all music + looping sounds
 * - pauseMusic(): Pauses current tracks (resume later)
 * - resumeMusic(): Continues paused tracks
 *
 * @see SoundObserver for game event → sound triggering
 */
class SoundManager {
private:
    /**
     * @brief Private constructor (singleton)
     */
    SoundManager();

    // Sound effect storage
    std::map<SoundEffect, sf::SoundBuffer> soundBuffers;        // WAV data
    std::map<SoundEffect, std::vector<sf::Sound>> soundPlayers; // 3 instances each

    // Music tracks
    sf::Music backgroundMusic;  // Gameplay music
    sf::Music menuMusic;        // Menu screen (unused in current build)
    sf::Music pauseScreenMusic; // Pause overlay

    // Looping sounds (as Music for seamless loops)
    sf::Music movementSound; // PacMan eating (waka-waka)
    bool movementSoundPlaying;

    sf::Music fearModeSound; // Ghost vulnerability
    bool fearModeSoundPlaying;

    // Volume settings
    float soundVolume; // 0-100, default 50
    float musicVolume; // 0-100, default 30

    bool soundsLoaded;

    /**
     * @brief Find available sound instance or reuse oldest
     *
     * @param effect Effect type to play
     * @return Pointer to sound instance, or nullptr if not loaded
     */
    sf::Sound* getAvailableSound(SoundEffect effect);

public:
    /**
     * @brief Get singleton instance
     *
     * @return Global SoundManager reference
     */
    static SoundManager& getInstance();

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    /**
     * @brief Load all sound files from directory
     *
     * Loads:
     * - 4 sound effects (fruit, ghost, death, level start)
     * - Background music
     * - Pause music
     * - Movement sound
     * - Fear mode sound
     *
     * @param soundDirectory Base directory path
     * @return true if at least some sounds loaded
     */
    bool loadSounds(const std::string& soundDirectory);

    /**
     * @brief Play one-shot sound effect
     *
     * Uses sound pooling (3 instances per effect).
     *
     * @param effect Effect to play
     */
    void playSound(SoundEffect effect);

    /**
     * @brief Start movement sound loop (waka-waka)
     *
     * Only starts if not already playing.
     * Used when PacMan moves and eats coins.
     */
    void startMovementSound();

    /**
     * @brief Stop movement sound
     */
    void stopMovementSound();

    /**
     * @brief Check if movement sound is playing
     *
     * @return true if active
     */
    bool isMovementSoundPlaying() const { return movementSoundPlaying; }

    /**
     * @brief Start fear mode sound loop
     *
     * Plays when fruit collected (ghosts vulnerable).
     */
    void startFearModeSound();

    /**
     * @brief Stop fear mode sound
     */
    void stopFearModeSound();

    /**
     * @brief Check if fear mode sound is playing
     *
     * @return true if active
     */
    bool isFearModeSoundPlaying() const { return fearModeSoundPlaying; }

    /**
     * @brief Play background music (gameplay)
     *
     * @param loop true to loop indefinitely (default)
     */
    void playBackgroundMusic(bool loop = true);

    /**
     * @brief Play menu music (unused in current build)
     *
     * @param loop true to loop
     */
    void playMenuMusic(bool loop = true);

    /**
     * @brief Play pause screen music
     *
     * @param loop true to loop
     */
    void playPauseMusic(bool loop = true);

    /**
     * @brief Stop all music and looping sounds
     *
     * Called on state transitions.
     */
    void stopMusic();

    /**
     * @brief Pause all currently playing music
     *
     * Can be resumed later with resumeMusic().
     */
    void pauseMusic();

    /**
     * @brief Resume all paused music
     *
     * Continues from pause point.
     */
    void resumeMusic();

    /**
     * @brief Set sound effects volume
     *
     * @param volume 0-100 (clamped)
     */
    void setSoundVolume(float volume);

    /**
     * @brief Set music volume
     *
     * @param volume 0-100 (clamped)
     */
    void setMusicVolume(float volume);

    /**
     * @brief Get current sound effects volume
     *
     * @return Volume 0-100
     */
    float getSoundVolume() const { return soundVolume; }

    /**
     * @brief Get current music volume
     *
     * @return Volume 0-100
     */
    float getMusicVolume() const { return musicVolume; }

    /**
     * @brief Mute sound effects (set volume to 0)
     */
    void muteSounds();

    /**
     * @brief Unmute sound effects (restore to 50%)
     */
    void unmuteSounds();

    /**
     * @brief Mute music (set volume to 0)
     */
    void muteMusic();

    /**
     * @brief Unmute music (restore to 30%)
     */
    void unmuteMusic();

    /**
     * @brief Check if sounds loaded successfully
     *
     * @return true if at least some files loaded
     */
    bool isLoaded() const { return soundsLoaded; }
};

} // namespace representation

#endif