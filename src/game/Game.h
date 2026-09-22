//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_GAME_H
#define COLLISIONCRISISHERKANSING_GAME_H
#include <vector>
#include <random>
#include "../core/GameSystem.h"
#include "../core/Profiler.h"
#include <SFML/Graphics.hpp>
#include "SpatialHash.h"

#ifndef SPATIALHASH_SIZE
#define SPATIALHASH_SIZE 32
#endif
#ifndef BALL_AMOUNT
#define BALL_AMOUNT 5000
#endif

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
    Profiler* profiler;

public:
    Game(const sf::Vector2u& windowSize, Profiler* profiler);

    std::function<void(AppLoopData*)> registerUpdateFunc() override;
    std::function<void(AppLoopData*)> registerDrawFunc() override;
    void stop() override;

    void update(const AppLoopData* data);


    void physicsUpdate(const sf::Vector2u& windowSize, const float& deltaTime);
    void updatePositions(const sf::Vector2u& windowSize, const float& deltaTime, const std::chrono::system_clock::time_point& frameStartTime);
    void doBallCollisions(const std::chrono::system_clock::time_point &frameStartTime) const;
    void doWallCollisions(const sf::Vector2u &windowSize, const std::chrono::system_clock::time_point &frameStartTime) const;
    void draw(const AppLoopData* data) const;

    static void ballToBallBucketToBucketCollision(const Bucket* bucket, const Bucket* otherBucket);
};


#endif //COLLISIONCRISISHERKANSING_GAME_H