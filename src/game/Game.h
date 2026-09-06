//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_GAME_H
#define COLLISIONCRISISHERKANSING_GAME_H
#include <vector>
#include <random>
#include "../core/GameSystem.h"

#include <SFML/Graphics.hpp>

struct Ball;

class Game: public GameSystem{
private:
    std::vector<Ball> balls;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> posDist;
    std::uniform_real_distribution<float> velDist;
    std::uniform_int_distribution<int> colorDist;
    std::uniform_real_distribution<float> radiusDist;

public:
    Game();
    std::function<void(AppLoopData*)> registerUpdateFunc() override;

    void update(AppLoopData* data);
    void physicsUpdate(const sf::Vector2u& windowSize, float deltaTime);
    void draw(sf::RenderWindow& window) const;
};


#endif //COLLISIONCRISISHERKANSING_GAME_H