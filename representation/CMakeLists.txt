find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_library(representation STATIC
        src/PacManView.cpp
        src/GhostView.cpp
        src/CoinView.cpp
        src/FruitView.cpp
        src/WallView.cpp
        src/Camera.cpp
        src/StateManager.cpp
        src/State.cpp
        src/MenuState.cpp
        src/LevelState.cpp
        src/PausedState.cpp
        src/VictoryState.cpp
        src/Game.cpp
        src/ConcreteFactory.cpp
        src/SpriteAtlas.cpp
        src/HUD.cpp
        src/SoundManager.cpp
        src/SoundObserver.cpp
        include/representation/HUD.h
        include/representation/SoundManager.h
        include/representation/SoundObserver.h
)

target_include_directories(representation PUBLIC include)
target_link_libraries(representation PUBLIC logic sfml-graphics sfml-window sfml-system sfml-audio)