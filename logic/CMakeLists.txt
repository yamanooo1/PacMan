add_library(logic STATIC
        src/core/EntityModel.cpp
        src/entities/PacMan.cpp
        src/entities/Ghost.cpp
        src/entities/Collectable.cpp
        src/entities/Coin.cpp
        src/entities/Fruit.cpp
        src/entities/Wall.cpp
        src/core/World.cpp
        src/systems/Score.cpp
        src/systems/Lives.cpp
        include/logic/entities/Exit.h
        src/patterns/Subject.cpp
)

target_include_directories(logic PUBLIC include)