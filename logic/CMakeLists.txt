add_library(logic STATIC
        src/EntityModel.cpp
        src/PacMan.cpp
        src/Ghost.cpp
        src/Collectable.cpp
        src/Coin.cpp
        src/Fruit.cpp
        src/Wall.cpp
        src/World.cpp
        src/Stopwatch.cpp
        src/Random.cpp
        src/Subject.cpp
        src/Score.cpp
        src/AbstractFactory.cpp
        src/Observer.cpp
)

target_include_directories(logic PUBLIC include)