find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_library(representation STATIC
        src/EntityView.cpp
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
)

target_include_directories(representation PUBLIC include)
target_link_libraries(representation PUBLIC logic sfml-graphics sfml-window sfml-system)