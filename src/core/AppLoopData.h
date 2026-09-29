//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_APPLOOPDATA_H
#define COLLISIONCRISISHERKANSING_APPLOOPDATA_H
#include "SFML/Graphics/RenderWindow.hpp"

struct AppLoopData {
public:
    std::chrono::system_clock::time_point startApplication;
    float deltaTime = 0;
    sf::RenderWindow* window = nullptr;
    AppLoopData(sf::RenderWindow* window, float deltaTime) {
        this->window = window;
        this->deltaTime = deltaTime;
    }
};

#endif //COLLISIONCRISISHERKANSING_APPLOOPDATA_H