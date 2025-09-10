cmake_minimum_required(VERSION 4.1)
project(space_invaders)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=-cppcoreguidelines-narrowing-conversions")

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

# Release build optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(HDRS
        include/Logger.h
        include/Game.h
        include/SpaceShip.h
        include/Laser.h
        include/Barrier.h
        include/Alien.h
        include/MysteryShip.h
        include/Explosion.h
        include/CellRect.h
        include/Entity.h
        include/ResourceManager.h
        include/states/GameStateManager.h
        include/states/GameOverState.h
        include/states/HighScoreState.h
        include/states/MenuState.h
        include/states/PausedState.h
        include/states/PlayingState.h
        include/states/QuitState.h
)

set(SRCS
        src/main.cpp
        src/Game.cpp
        src/SpaceShip.cpp
        src/Laser.cpp
        src/Barrier.cpp
        src/Alien.cpp
        src/MysteryShip.cpp
        src/Explosion.cpp
        src/Entity.cpp
        src/ResourceManager.cpp
        src/CellRect.cpp
        src/states/GameStateManager.cpp
        src/states/GameOverState.cpp
        src/states/HighScoreState.cpp
        src/states/MenuState.cpp
        src/states/PausedState.cpp
        src/states/PlayingState.cpp
        src/states/QuitState.cpp
)

add_executable(space_invaders ${HDRS} ${SRCS})

# Handle cross-compilation for Windows
if(WIN32)
    # Path to your extracted Raylib Windows binaries
    set(RAYLIB_PATH "${CMAKE_SOURCE_DIR}/raylib-5.5_win64_msvc16")

    target_include_directories(space_invaders PRIVATE
            include
            ${RAYLIB_PATH}/include
    )

    target_link_directories(space_invaders PRIVATE ${RAYLIB_PATH}/lib)
    target_link_libraries(space_invaders raylib opengl32 gdi32 winmm)
else()
    target_link_libraries(space_invaders raylib)
    target_include_directories(space_invaders PRIVATE include)
endif()
