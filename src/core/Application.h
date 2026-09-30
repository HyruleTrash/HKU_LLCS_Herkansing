//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_SYSTEM_H
#define COLLISIONCRISISHERKANSING_SYSTEM_H
#include <SFML/Graphics.hpp>

#include "AppLoopData.h"
#include "GameSystem.h"
#include "ThreadPool.h"

/**
 * @brief Highest level layer, manages the window
 */
class Application {
private:
    std::vector<GameSystem*> registeredSystems;
    std::vector<std::function<void(AppLoopData*)>> registeredUpdateCalls;
    std::vector<std::function<void(AppLoopData*)>> registeredDrawCalls;
    std::vector<std::function<void(const sf::Event&, AppLoopData*)>> registeredEventCalls;
    sf::RenderWindow* window = nullptr;
    sf::Clock* clock = nullptr;
    ThreadPool* threadPool = nullptr;
    AppLoopData* appLoopData = nullptr;

  public:
    void init();
    void triggerAppLoop() const;
    void registerSystem(GameSystem* sys);

    [[nodiscard]] sf::RenderWindow* getWindow() const;
    [[nodiscard]] ThreadPool* getThreadPool() const;
};

#endif //COLLISIONCRISISHERKANSING_SYSTEM_H