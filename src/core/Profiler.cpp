//
// Created by Lucas on 10/09/2026.
//

#include "Profiler.h"

#include <iostream>

#include "imgui-SFML.h"
#include <thread>

Profiler::Profiler(sf::RenderWindow &window) {
    this->running = ImGui::SFML::Init(window);
    this->profilerWindow = new ImGuiUtils::ProfilersWindow(1.0f / 60.0f);
}

std::function<void(AppLoopData *)> Profiler::registerUpdateFunc() {
    std::cout << "Registering Profiler update loop to app loop.\n";
    return std::bind(&Profiler::update, this, std::placeholders::_1);
}

std::function<void(AppLoopData *)> Profiler::registerDrawFunc() {
    std::cout << "Registering Profiler draw call to app.\n";
    return std::bind(&Profiler::draw, this, std::placeholders::_1);
}

void Profiler::stop() {
    ImGui::SFML::Shutdown();
}

void Profiler::update(AppLoopData* data) {
    if (this->running == false) return;

    ImGui::SFML::Update(*data->window, sf::seconds(data->deltaTime));

    profilerWindow->cpuGraph.LoadFrameData(cpuTasks.data(), cpuTasks.size());
    profilerWindow->Render();
    cpuTasks.clear();
}

void Profiler::draw(AppLoopData* data) {
    if (this->running == false) return;
    ImGui::SFML::Render(*data->window);
}
