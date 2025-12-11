add_library(logic STATIC
        src/EntityModel.cpp
        src/PacMan.cpp
        src/Ghost.cpp
        src/Collectable.cpp
        src/Coin.cpp
        src/Fruit.cpp
        src/Wall.cpp
        src/World.cpp
        src/Score.cpp
        src/Lives.cpp
        include/logic/Exit.h
        src/Subject.cpp
)

target_include_directories(logic PUBLIC include)