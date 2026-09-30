//
// Created by Lucas on 06/09/2026.
//

#include "Application.h"
#include "imgui-SFML.h"

#include <iostream>

/**
 * @brief Creates the components the application needs to be used
 */
void Application::init() {
    this->window = new sf::RenderWindow( sf::VideoMode( { 800, 800 } ), "Collision Crisis" );
    this->clock = new sf::Clock();
    this->threadPool = new ThreadPool(4);

    this->appLoopData = new AppLoopData();
    this->appLoopData->startApplication = std::chrono::high_resolution_clock::now();
    this->appLoopData->deltaTime = 0;
    this->appLoopData->window = this->window;
    this->appLoopData->threadPool = this->threadPool;

    std::cout << "Initializing main application, \nCreating: \nwindow, \nLoopdata, \nand program clock.\n\n";

    this->threadPool->init();
}

/**
 * @brief Triggers the start of the update and render loop
 */
void Application::triggerAppLoop() const {
    if (window == nullptr) return;

    while ( window->isOpen() )
    {
        sf::Time elapsed = clock->restart();
        this->appLoopData->deltaTime = elapsed.asSeconds();

        while ( const std::optional event = window->pollEvent() ) {
            if ( event->is<sf::Event::Closed>() ) {
                this->threadPool->shutdown();
                for (GameSystem* sys : this->registeredSystems) sys->stop();
                window->close();
                break;
            }
            for (const auto& func : registeredEventCalls) func(*event, this->appLoopData);
        }

        window->clear();
        for (const auto& func: registeredUpdateCalls) func(this->appLoopData);
        for (const auto& func: registeredDrawCalls) func(this->appLoopData);
        window->display();
    }
}

/**
 * @brief subscribe a game to the update/app loop
 * @param sys, game system, to add to update/app loop
 */
void Application::registerSystem(GameSystem* sys) {
    this->registeredSystems.push_back(sys);
    if (const auto eventFunc = sys->registerEventFunc())
        registeredEventCalls.push_back(eventFunc);
    if (const auto updateFunc = sys->registerUpdateFunc())
        registeredUpdateCalls.push_back(updateFunc);
    if (const auto drawFunc = sys->registerDrawFunc())
        registeredDrawCalls.push_back(drawFunc);
}

sf::RenderWindow* Application::getWindow() const { return this->window; }
ThreadPool* Application::getThreadPool() const { return this->threadPool; }
