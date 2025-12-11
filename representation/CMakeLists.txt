find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_library(representation STATIC
        src/entities/PacManView.cpp
        src/entities/GhostView.cpp
        src/entities/CoinView.cpp
        src/entities/FruitView.cpp
        src/entities/WallView.cpp
        src/core/Camera.cpp
        src/states/StateManager.cpp
        src/states/MenuState.cpp
        src/states/LevelState.cpp
        src/states/PausedState.cpp
        src/states/VictoryState.cpp
        src/core/Game.cpp
        src/core/ConcreteFactory.cpp
        src/core/SpriteAtlas.cpp
        src/systems/HUD.cpp
        src/systems/SoundManager.cpp
        src/systems/SoundObserver.cpp
        include/representation/systems/HUD.h
        include/representation/systems/SoundManager.h
        include/representation/systems/SoundObserver.h
)

target_include_directories(representation PUBLIC include)
target_link_libraries(representation PUBLIC logic sfml-graphics sfml-window sfml-system sfml-audio)