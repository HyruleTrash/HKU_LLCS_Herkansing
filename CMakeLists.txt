cmake_minimum_required(VERSION 3.28)
project(CollisionCrisisHerkansing LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(CollisionCrisisHerkansing src/main.cpp
        src/core/Application.cpp
        src/core/Application.h
        src/game/Game.cpp
        src/game/Game.h
        src/game/entities/Ball.h
        src/core/GameSystem.h
        src/core/AppLoopData.h)
target_compile_features(CollisionCrisisHerkansing PRIVATE cxx_std_17)
target_link_libraries(CollisionCrisisHerkansing PRIVATE SFML::Graphics)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CollisionCrisisHerkansing)