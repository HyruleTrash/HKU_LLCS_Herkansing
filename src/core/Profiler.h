//
// Created by Lucas on 10/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_PROFILER_H
#define COLLISIONCRISISHERKANSING_PROFILER_H
#include "GameSystem.h"
#include "../../include/ImGuiProfilerRenderer.h"

class Profiler : public GameSystem {
private:
    ImGuiUtils::ProfilersWindow* profilerWindow;
    std::vector<legit::ProfilerTask> cpuTasks;

public:
    Profiler(sf::RenderWindow& window);
    std::function<void(AppLoopData*)> registerUpdateFunc() override;
    std::function<void(AppLoopData*)> registerDrawFunc() override;
    void stop() override;

    void update(AppLoopData* data);
    void draw(AppLoopData* data);

    bool running;
};


#endif //COLLISIONCRISISHERKANSING_PROFILER_H