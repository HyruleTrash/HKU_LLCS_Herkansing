//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_GAME_H
#define COLLISIONCRISISHERKANSING_GAME_H
#include <vector>
#include <random>
#include "../core/GameSystem.h"

#include <SFML/Graphics.hpp>

#include "SpatialHash.h"

struct Ball;

class Game final : public GameSystem{
private:
    std::vector<Ball*> balls;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> posDist;
    std::uniform_real_distribution<float> velDist;
    std::uniform_int_distribution<> colorDist;
    std::uniform_real_distribution<float> radiusDist;
    SpatialHash spatialHash;

public:
    Game(const sf::Vector2u& windowSize);
    std::function<void(AppLoopData*)> registerUpdateFunc() override;
    std::function<void(AppLoopData*)> registerDrawFunc() override;
    void stop() override;

    void update(AppLoopData* data);
    void physicsUpdate(const sf::Vector2u& windowSize, const float& deltaTime);
    void draw(const AppLoopData* data) const;
};


#endif //COLLISIONCRISISHERKANSING_GAME_H