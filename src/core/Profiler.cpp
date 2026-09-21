//
// Created by Lucas on 10/09/2026.
//

#include "Profiler.h"

#include <iostream>

#include "imgui-SFML.h"
#include <thread>

template <typename Clock, typename Duration>
float SecondsSince(std::chrono::time_point<Clock, Duration> start)
{
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();
}

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

void Profiler::update(AppLoopData *data) {
    if (this->running == false) return;

    cpuTasks.clear();

    const auto frameStartTime = std::chrono::high_resolution_clock::now();
    ImGui::SFML::Update(*data->window, sf::seconds(data->deltaTime));

    const float t1Start = SecondsSince(frameStartTime);
    cpuTasks.push_back({ t1Start, t1Start, "Update Logic", IM_COL32(255, 100, 100, 255) });

    profilerWindow->cpuGraph.LoadFrameData(cpuTasks.data(), cpuTasks.size());

    profilerWindow->Render();
}

void Profiler::draw(AppLoopData *data) {
    if (this->running == false) return;
    ImGui::SFML::Render(*data->window);
}
