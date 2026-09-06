//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_SYSTEM_H
#define COLLISIONCRISISHERKANSING_SYSTEM_H
#include <SFML/Graphics.hpp>

#include "AppLoopData.h"
#include "GameSystem.h"

/**
 * @brief Highest level layer, manages the window
 */
class Application {
private:
    sf::RenderWindow* window = nullptr;
    AppLoopData* appLoopData = nullptr;
    std::vector<std::function<void(AppLoopData*)>> registeredSystems;
    sf::Clock* clock = nullptr;

public:
    void init();
    void triggerAppLoop() const;
    void registerSystem(GameSystem* sys);
};

#endif //COLLISIONCRISISHERKANSING_SYSTEM_H