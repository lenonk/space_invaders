cmake_minimum_required(VERSION 4.1)
project(space_invaders)

set(CMAKE_CXX_STANDARD 26)

# Release build optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(HDRS
        include/Game.h
        include/SpaceShip.h
        include/Laser.h
        include/Barrier.h
        include/Alien.h
        include/MysteryShip.h
)

set(SRCS
        src/main.cpp
        src/Game.cpp
        src/SpaceShip.cpp
        src/Laser.cpp
        src/Barrier.cpp
        src/Alien.cpp
        src/MysteryShip.cpp
)

add_executable(space_invaders ${SRCS})

target_link_libraries(space_invaders raylib)
target_include_directories(space_invaders PRIVATE include)
