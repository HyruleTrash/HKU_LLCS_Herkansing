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
    this->appLoopData = new AppLoopData(window, 0);
    this->clock = new sf::Clock();

    std::cout << "Initializing main application, \nCreating: \nwindow, \nLoopdata, \nand program clock.\n\n";
}

/**
 * @brief Triggers the start of the update and render loop
 */
void Application::triggerAppLoop() const {
    if (window == nullptr) return;

    this->appLoopData->startApplication = std::chrono::high_resolution_clock::now();

    while ( window->isOpen() )
    {
        sf::Time elapsed = clock->restart();
        this->appLoopData->deltaTime = elapsed.asSeconds();

        while ( const std::optional event = window->pollEvent() ) {
            if ( event->is<sf::Event::Closed>() ) window->close();
            for (const auto& func : registeredEventSystems) func(*event, this->appLoopData);
        }

        window->clear();
        for (const auto& func: registeredUpdateSystems) func(this->appLoopData);
        for (const auto& func: registeredDrawSystems) func(this->appLoopData);
        window->display();
    }
}

/**
 * @brief subscribe a game to the update/app loop
 * @param sys, game system, to add to update/app loop
 */
void Application::registerSystem(GameSystem *sys) {
    if (const auto eventFunc = sys->registerEventFunc())
        registeredEventSystems.push_back(eventFunc);
    if (const auto updateFunc = sys->registerUpdateFunc())
        registeredUpdateSystems.push_back(updateFunc);
    if (const auto drawFunc = sys->registerDrawFunc())
        registeredDrawSystems.push_back(drawFunc);
}

sf::RenderWindow* Application::getWindow() const {
    return this->window;
}
