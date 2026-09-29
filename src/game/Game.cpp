//
// Created by Lucas on 06/09/2026.
//

#include "Game.h"
#include <iostream>

template <typename Clock, typename Duration>
float SecondsSince(std::chrono::time_point<Clock, Duration> start)
{
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();
}

Game::Game(Profiler* profiler) {
    this->profiler = profiler;

    this->shape = sf::CircleShape(100.f);
    shape.setFillColor(sf::Color::Green);
}

std::function<void(AppLoopData*)> Game::registerUpdateFunc() {
    std::cout << "Registering main game update loop to app loop.\n";
    return std::bind(&Game::update, this, std::placeholders::_1);
}

std::function<void(AppLoopData *)> Game::registerDrawFunc() {
    std::cout << "Registering main game draw call to app.\n";
    return std::bind(&Game::draw, this, std::placeholders::_1);
}

void Game::update(const AppLoopData* data) {
    constexpr float speed = 1;
    constexpr float distance = 100;
    const auto windowSize = data->window->getSize();
    const auto shapeSize = shape.getGlobalBounds().size;
    shape.setPosition({
        static_cast<float>(windowSize.x / 2) - shapeSize.x / 2,
        static_cast<float>(windowSize.y / 2) - shapeSize.y / 2,
    });
    shape.move({0, static_cast<float>(sin(SecondsSince(data->startApplication) * speed) * distance)});
}

void Game::draw(const AppLoopData *data) const {
    data->window->draw(shape);
}
