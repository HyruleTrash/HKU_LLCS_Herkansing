cmake_minimum_required(VERSION 3.28)

set(PROJECT_NAME CollisionCrisisHerkansing)

project(${PROJECT_NAME} LANGUAGES CXX)

set(SFML_VERSION 3.1.0)
set(IMGUI_VERSION 1.91.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# --- SFML ---
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG ${SFML_VERSION}
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

# --- ImGui ---
FetchContent_Declare(
        imgui
        URL "https://github.com/ocornut/imgui/archive/v${IMGUI_VERSION}.zip"
)
FetchContent_MakeAvailable(imgui)

# --- ImGui-SFML ---
FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
        GIT_TAG        v3.0
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)

FetchContent_MakeAvailable(imgui-sfml)

# --- GLM ---
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(glm)

# --- Executable & Targets ---
add_executable(${PROJECT_NAME} src/main.cpp
        include/ImGuiProfilerRenderer.h
        src/core/Application.cpp
        src/core/Application.h
        src/game/Game.cpp
        src/game/Game.h
        src/game/entities/Ball.h
        src/core/GameSystem.h
        src/core/AppLoopData.h
        src/core/Profiler.cpp
        src/core/Profiler.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE
        SFML::Graphics
        glm::glm
)
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui-SFML::ImGui-SFML)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})