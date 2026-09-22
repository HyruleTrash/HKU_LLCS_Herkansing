//
// Created by Lucas on 06/09/2026.
//

#include "Game.h"
#include <iostream>
#include "entities/Ball.h"

template <typename Clock, typename Duration>
float SecondsSince(std::chrono::time_point<Clock, Duration> start)
{
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();
}

Game::Game(const sf::Vector2u& windowSize, Profiler* profiler) : spatialHash(SpatialHash(std::make_tuple(SPATIALHASH_SIZE, SPATIALHASH_SIZE))) {
    gen = std::mt19937(rd());
    posDist = std::uniform_real_distribution(5.0f, 795.0f);
    velDist = std::uniform_real_distribution(-200.0f, 200.0f);
    colorDist = std::uniform_int_distribution(0, 255);
    radiusDist = std::uniform_real_distribution(2.5f, 2.5f);

    this->profiler = profiler;

    // Generate random balls
    for (int i = 0; i < BALL_AMOUNT; ++i) {
        const sf::Color randomColor(colorDist(gen), colorDist(gen), colorDist(gen));
        const auto ball = new Ball(
            posDist(gen), posDist(gen),     // position
            radiusDist(gen),                     // radius
            randomColor,                            // color
            velDist(gen), velDist(gen)     // velocity
        );
        this->balls.push_back(ball);
        this->spatialHash.moveBallIntoBucket(ball, windowSize);
    }
}

std::function<void(AppLoopData*)> Game::registerUpdateFunc() {
    std::cout << "Registering main game update loop to app loop.\n";
    return std::bind(&Game::update, this, std::placeholders::_1);
}

std::function<void(AppLoopData *)> Game::registerDrawFunc() {
    std::cout << "Registering main game draw call to app.\n";
    return std::bind(&Game::draw, this, std::placeholders::_1);
}

void Game::stop() {
    // nothing to remove
}

void Game::update(const AppLoopData* data) {
    this->physicsUpdate(data->window->getSize(), data->deltaTime);
}

void Game::physicsUpdate(const sf::Vector2u& windowSize, const float& deltaTime) {
    const auto frameStartTime = std::chrono::high_resolution_clock::now();
    updatePositions(windowSize, deltaTime, frameStartTime);
    doBallCollisions(frameStartTime);
    doWallCollisions(windowSize, frameStartTime);
}

void Game::updatePositions(const sf::Vector2u& windowSize, const float& deltaTime,
    const std::chrono::system_clock::time_point& frameStartTime) {
    const float startTime = SecondsSince(frameStartTime);
    for (const auto& ball : balls) {
        ball->move(ball->velocity * deltaTime);
        spatialHash.moveBallIntoBucket(ball, windowSize);
    }
    const float endTime = SecondsSince(frameStartTime);
    this->profiler->cpuTasks.push_back({ startTime, endTime, "Update ball positions", IM_COL32(255, 100, 100, 255) });

}

void Game::doBallCollisions(const std::chrono::system_clock::time_point &frameStartTime) const {
    const float startTime = SecondsSince(frameStartTime);
    constexpr std::array<int, 5> neighborOffsets = {
        0,                      // Current bucket
        1,                      // Right
        SPATIALHASH_SIZE - 1,   // Bottom-Left
        SPATIALHASH_SIZE,       // Bottom
        SPATIALHASH_SIZE + 1    // Bottom-Right
    };
    for (size_t k = 0; k < this->spatialHash.buckets.size(); ++k) {
        const auto bucket = this->spatialHash.buckets[k];

        for (const int offset : neighborOffsets) {
            // ReSharper disable once CppTooWideScopeInitStatement
            const size_t targetIndex = k + offset;

            if (targetIndex >= this->spatialHash.buckets.size()) continue;
            const auto otherBucket = this->spatialHash.buckets[targetIndex];
            ballToBallBucketToBucketCollision(&bucket, &otherBucket);
        }
    }
    const float endTime = SecondsSince(frameStartTime);
    this->profiler->cpuTasks.push_back({ startTime, endTime, "Update ball-to-ball collisions", IM_COL32(100, 255, 100, 255) });
}

void Game::doWallCollisions(const sf::Vector2u &windowSize, const std::chrono::system_clock::time_point &frameStartTime) const {
    const float startTime = SecondsSince(frameStartTime);
    for (const auto bucket: this->spatialHash.edgeBuckets) {
        for (const auto& ball : bucket->content) {
            const sf::Vector2f pos = ball->getPositionVec2();
            const float radius = ball->getRadius();

            // Bounce off walls
            if (pos.x - radius <= 0 || pos.x + radius >= windowSize.x) {
                ball->velocity.x = -ball->velocity.x;

                // Clamp position to prevent sticking
                if (pos.x - radius <= 0)    ball->updatePosition(sf::Vector2f(radius, pos.y));
                else                        ball->updatePosition(sf::Vector2f(windowSize.x - radius, pos.y));
            }

            if (pos.y - radius <= 0 || pos.y + radius >= windowSize.y) {
                ball->velocity.y = -ball->velocity.y;

                // Clamp position to prevent sticking
                if (pos.y - radius <= 0)    ball->updatePosition(sf::Vector2f(pos.x, radius));
                else                        ball->updatePosition(sf::Vector2f(pos.x, windowSize.y - radius));
            }
        }
    }
    const float endTime = SecondsSince(frameStartTime);
    this->profiler->cpuTasks.push_back({ startTime, endTime, "Update wall collisions", IM_COL32(100, 100, 255, 255) });
}

void Game::draw(const AppLoopData *data) const {
    for (const auto& ball : balls) ball->draw(data->window);
}

void Game::ballToBallBucketToBucketCollision(const Bucket* bucket, const Bucket* otherBucket) {
    for (const auto ball1 : bucket->content) {
        for (const auto ball2 : otherBucket->content) {
            if (ball1 == ball2) continue;

            const sf::Vector2f pos1 = ball1->getPositionVec2();
            const sf::Vector2f pos2 = ball2->getPositionVec2();
            const float radius1 = ball1->getRadius();
            const float radius2 = ball2->getRadius();

            // Calculate distance between centers
            const sf::Vector2f delta = pos2 - pos1;
            const float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            const float minDistance = radius1 + radius2;

            if (distance >= minDistance || distance <= 0) continue;
            // Normalize collision vector
            const sf::Vector2f normal = delta / distance;

            // Separate balls to prevent overlap
            const float overlap = minDistance - distance;
            const sf::Vector2f separation = normal * (overlap * 0.5f);
            ball1->updatePosition(pos1 - separation);
            ball2->updatePosition(pos2 + separation);

            // Calculate relative velocity
            const sf::Vector2f relativeVel = ball2->velocity - ball1->velocity;
            const float velAlongNormal = relativeVel.x * normal.x + relativeVel.y * normal.y;

            // Don't resolve if velocities are separating
            if (velAlongNormal > 0) continue;

            // Apply collision response (elastic collision)
            constexpr float restitution = 0.0f; // Bounce factor (0 = no bounce, 1 = perfect bounce)
            const float impulse = -(1 + restitution) * velAlongNormal;

            // Assume equal mass for simplicity
            const sf::Vector2f impulseVector = impulse * normal;
            ball1->velocity -= impulseVector;
            ball2->velocity += impulseVector;
        }
    }
}
