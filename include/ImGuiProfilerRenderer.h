#pragma once

#include <algorithm>

#include "ProfilerTask.h"
#include "imgui.h"
#include <array>
#include <chrono>
#include <glm/vec2.hpp>
#include <map>
#include <sstream>
#include <vector>

namespace ImGuiUtils {

inline glm::vec2 Vec2(const ImVec2 vec) { return glm::vec2(vec.x, vec.y); }

class ProfilerGraph {
  public:
    int frameWidth;
    int frameSpacing;
    bool useColoredLegendText;

    ProfilerGraph(const size_t framesCount) {
        frames.resize(framesCount);
        for (auto& frame : frames)
            frame.tasks.reserve(100);
        frameWidth = 3;
        frameSpacing = 1;
        useColoredLegendText = false;
    }

    void LoadFrameData(const legit::ProfilerTask* tasks, const size_t count) {
        auto& currFrame = frames[currFrameIndex];
        currFrame.tasks.resize(0);
        currFrame.totalTime = 0.0f;

        for (size_t taskIndex = 0; taskIndex < count; taskIndex++) {
            if (taskIndex == 0) currFrame.tasks.push_back(tasks[taskIndex]);
            else {
                if (tasks[taskIndex - 1].color != tasks[taskIndex].color || tasks[taskIndex - 1].name != tasks[taskIndex].name)
                    currFrame.tasks.push_back(tasks[taskIndex]);
                else
                    currFrame.tasks.back().endTime = tasks[taskIndex].endTime;
            }

            currFrame.totalTime += tasks[taskIndex].endTime - tasks[taskIndex].startTime;
        }
        currFrame.taskStatsIndex.resize(currFrame.tasks.size());

        for (size_t taskIndex = 0; taskIndex < currFrame.tasks.size(); taskIndex++) {
            auto& task = currFrame.tasks[taskIndex];
            auto it = taskNameToStatsIndex.find(task.name);

            if (it == taskNameToStatsIndex.end()) {
                taskNameToStatsIndex[task.name] = taskStats.size();
                TaskStats taskStat;
                taskStat.maxTime = -1.0;
                taskStats.push_back(taskStat);
            }
            currFrame.taskStatsIndex[taskIndex] = taskNameToStatsIndex[task.name];
        }

        currFrameIndex = (currFrameIndex + 1) % frames.size();
        RebuildTaskStats(currFrameIndex, 300);
    }

    float GetTotalTaskTime(const int frameIndexOffset) { return frames[GetCurrFrameIndex(frameIndexOffset)].totalTime; }

    void RenderTimings(const int graphWidth, const int legendWidth, const int height, const int frameIndexOffset, const float maxFrameTime) {
        // Safety check: skip rendering drawlist if available space is invalid or collapsed
        if (graphWidth <= 0 || height <= 0) return;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const glm::vec2 widgetPos = Vec2(ImGui::GetCursorScreenPos());

        RenderGraph(drawList, widgetPos, glm::vec2(graphWidth, height), frameIndexOffset, maxFrameTime);
        RenderLegend(drawList, widgetPos + glm::vec2(graphWidth, 0.0f), glm::vec2(legendWidth, height), frameIndexOffset, maxFrameTime);
        ImGui::Dummy(ImVec2(static_cast<float>(graphWidth + legendWidth), static_cast<float>(height)));
    }

  private:
    size_t GetCurrFrameIndex(const size_t frameIndexOffset) { return (currFrameIndex - frameIndexOffset - 1 + 2 * frames.size()) % frames.size(); }

    void RebuildTaskStats(const size_t endFrame, const size_t framesCount) {
        for (auto& taskStat : taskStats) {
            taskStat.maxTime = -1.0f;
            taskStat.priorityOrder = static_cast<size_t>(-1);
            taskStat.onScreenIndex = static_cast<size_t>(-1);
        }

        for (size_t frameNumber = 0; frameNumber < framesCount; frameNumber++) {
            const size_t frameIndex = (endFrame - 1 - frameNumber + frames.size()) % frames.size();
            auto& frame = frames[frameIndex];
            for (size_t taskIndex = 0; taskIndex < frame.tasks.size(); taskIndex++) {
                const auto& task = frame.tasks[taskIndex];
                auto& stats = taskStats[frame.taskStatsIndex[taskIndex]];
                stats.maxTime = std::max(stats.maxTime, task.endTime - task.startTime);
            }
        }
        std::vector<size_t> statPriorities(taskStats.size());
        for (size_t statIndex = 0; statIndex < taskStats.size(); statIndex++) statPriorities[statIndex] = statIndex;

        std::sort(statPriorities.begin(), statPriorities.end(), [this](const size_t left, const size_t right) { return taskStats[left].maxTime > taskStats[right].maxTime; });
        for (size_t statNumber = 0; statNumber < taskStats.size(); statNumber++) {
            const size_t statIndex = statPriorities[statNumber];
            taskStats[statIndex].priorityOrder = statNumber;
        }
    }

    void RenderGraph(ImDrawList* drawList, const glm::vec2 graphPos, const glm::vec2 graphSize, const size_t frameIndexOffset, const float maxFrameTime) {
        Rect(drawList, graphPos, graphPos + graphSize, 0xffffffff, false);
        constexpr float heightThreshold = 1.0f;

        for (size_t frameNumber = 0; frameNumber < frames.size(); frameNumber++) {
            const size_t frameIndex = GetCurrFrameIndex(frameIndexOffset + frameNumber);

            glm::vec2 framePos = graphPos + glm::vec2(graphSize.x - 1 - frameWidth - (frameWidth + frameSpacing) * frameNumber, graphSize.y - 1);
            if (framePos.x < graphPos.x + 1) break;

            glm::vec2 taskPos = framePos + glm::vec2(0.0f, 0.0f);
            auto& frame = frames[frameIndex];
            for (const auto& task : frame.tasks) {
                const float taskStartHeight = (static_cast<float>(task.startTime) / maxFrameTime) * graphSize.y;
                const float taskEndHeight = (static_cast<float>(task.endTime) / maxFrameTime) * graphSize.y;

                if (abs(taskEndHeight - taskStartHeight) > heightThreshold)
                    Rect(drawList, taskPos + glm::vec2(0.0f, -taskStartHeight), taskPos + glm::vec2(frameWidth, -taskEndHeight), task.color, true);
            }
        }
    }

    void RenderLegend(ImDrawList* drawList, glm::vec2 legendPos, glm::vec2 legendSize, size_t frameIndexOffset, float maxFrameTime) {
        float markerLeftRectMargin = 3.0f;
        float markerLeftRectWidth = 5.0f;
        float markerMidWidth = 30.0f;
        float markerRightRectWidth = 10.0f;
        float markerRigthRectMargin = 3.0f;
        float markerRightRectHeight = 10.0f;
        float markerRightRectSpacing = 4.0f;
        float nameOffset = 30.0f;
        auto textMargin = glm::vec2(5.0f, -3.0f);

        auto& currFrame = frames[GetCurrFrameIndex(frameIndexOffset)];
        size_t maxTasksCount = static_cast<size_t>(legendSize.y / (markerRightRectHeight + markerRightRectSpacing));

        for (auto& taskStat : taskStats)
            taskStat.onScreenIndex = static_cast<size_t>(-1);

        size_t tasksToShow = std::min<size_t>(taskStats.size(), maxTasksCount);
        size_t tasksShownCount = 0;
        for (size_t taskIndex = 0; taskIndex < currFrame.tasks.size(); taskIndex++) {
            auto& task = currFrame.tasks[taskIndex];
            auto& stat = taskStats[currFrame.taskStatsIndex[taskIndex]];

            if (stat.priorityOrder >= tasksToShow) continue;

            if (stat.onScreenIndex == static_cast<size_t>(-1)) {
                stat.onScreenIndex = tasksShownCount++;
            } else continue;

            float taskStartHeight = (static_cast<float>(task.startTime) / maxFrameTime) * legendSize.y;
            float taskEndHeight = (static_cast<float>(task.endTime) / maxFrameTime) * legendSize.y;

            glm::vec2 markerLeftRectMin = legendPos + glm::vec2(markerLeftRectMargin, legendSize.y);
            glm::vec2 markerLeftRectMax = markerLeftRectMin + glm::vec2(markerLeftRectWidth, 0.0f);
            markerLeftRectMin.y -= taskStartHeight;
            markerLeftRectMax.y -= taskEndHeight;

            glm::vec2 markerRightRectMin = legendPos + glm::vec2(markerLeftRectMargin + markerLeftRectWidth + markerMidWidth, legendSize.y - markerRigthRectMargin - (markerRightRectHeight + markerRightRectSpacing) * stat.onScreenIndex);
            glm::vec2 markerRightRectMax = markerRightRectMin + glm::vec2(markerRightRectWidth, -markerRightRectHeight);
            RenderTaskMarker(drawList, markerLeftRectMin, markerLeftRectMax, markerRightRectMin, markerRightRectMax, task.color);

            uint32_t textColor = useColoredLegendText ? task.color : legit::Colors::imguiText;

            float taskTimeMs = static_cast<float>(task.endTime - task.startTime);
            std::ostringstream timeText;
            timeText.precision(2);
            timeText << std::fixed << std::string("[") << (taskTimeMs * 1000.0f);

            Text(drawList, markerRightRectMax + textMargin, textColor, timeText.str().c_str());
            Text(drawList, markerRightRectMax + textMargin + glm::vec2(nameOffset, 0.0f), textColor, (std::string("ms] ") + task.name).c_str());
        }
    }

    static void Rect(ImDrawList* drawList, const glm::vec2 minPoint, const glm::vec2 maxPoint, const uint32_t col, const bool filled = true) {
        if (filled) drawList->AddRectFilled(ImVec2(minPoint.x, minPoint.y), ImVec2(maxPoint.x, maxPoint.y), col);
        else drawList->AddRect(ImVec2(minPoint.x, minPoint.y), ImVec2(maxPoint.x, maxPoint.y), col);
    }

    static void Text(ImDrawList* drawList, const glm::vec2 point, const uint32_t col, const char* text) { drawList->AddText(ImVec2(point.x, point.y), col, text); }

    static void RenderTaskMarker(ImDrawList* drawList, const glm::vec2 leftMinPoint, const glm::vec2 leftMaxPoint, const glm::vec2 rightMinPoint, const glm::vec2 rightMaxPoint, const uint32_t col) {
        Rect(drawList, leftMinPoint, leftMaxPoint, col, true);
        Rect(drawList, rightMinPoint, rightMaxPoint, col, true);
        const std::array<ImVec2, 4> points = {ImVec2(leftMaxPoint.x, leftMinPoint.y), ImVec2(leftMaxPoint.x, leftMaxPoint.y), ImVec2(rightMinPoint.x, rightMaxPoint.y), ImVec2(rightMinPoint.x, rightMinPoint.y)};
        drawList->AddConvexPolyFilled(points.data(), static_cast<int>(points.size()), col);
    }

    struct FrameData {
        std::vector<legit::ProfilerTask> tasks;
        std::vector<size_t> taskStatsIndex;
        float totalTime;
    };

    struct TaskStats {
        double maxTime;
        size_t priorityOrder;
        size_t onScreenIndex;
    };
    std::vector<TaskStats> taskStats;
    std::map<std::string, size_t> taskNameToStatsIndex;
    std::vector<FrameData> frames;
    size_t currFrameIndex = 0;
};

class ProfilersWindow {
  public:
    ProfilersWindow(const float maxFrameTime = 1.0f / 60.0f) : cpuGraph(300), gpuGraph(300), maxFrameTime(maxFrameTime) {
        stopProfiling = false;
        frameOffset = 0;
        frameWidth = 3;
        frameSpacing = 1;
        useColoredLegendText = true;
        prevFpsFrameTime = std::chrono::system_clock::now();
        fpsFramesCount = 0;
        avgFrameTime = 1.0f;
    }

    void Render() {
        fpsFramesCount++;
        const auto currFrameTime = std::chrono::system_clock::now();
        if (const float fpsDeltaTime = std::chrono::duration<float>(currFrameTime - prevFpsFrameTime).count(); fpsDeltaTime > 0.5f) {
            this->avgFrameTime = fpsDeltaTime / static_cast<float>(fpsFramesCount);
            fpsFramesCount = 0;
            prevFpsFrameTime = currFrameTime;
        }

        std::stringstream title;
        title.precision(2);
        title << std::fixed << "Legit profiler [" << 1.0f / avgFrameTime << "fps\t"
              << " cpu: " << cpuGraph.GetTotalTaskTime(frameOffset) * 1000.0f << "ms gpu: " << gpuGraph.GetTotalTaskTime(frameOffset) * 1000.0f << "ms]###ProfilerWindow";

        ImGui::SetNextWindowSize(ImVec2(600.0f, 400.0f), ImGuiCond_FirstUseEver);

        // Standard flags: scrollbars enabled, moving/resizing/collapsing permitted
        if (ImGui::Begin(title.str().c_str(), nullptr, 0)) {
            const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            const float itemSpacingY = ImGui::GetStyle().ItemSpacing.y;

            // Reserve height for the control section (~85px)
            const float controlsHeight = 85.0f;
            const bool showControls = canvasSize.y > (controlsHeight + 60.0f);

            // Compute remaining height for graphs
            const float availableGraphAreaY = showControls ? (canvasSize.y - controlsHeight - itemSpacingY) : canvasSize.y;

            constexpr int legendWidth = 200;
            const int graphWidth = static_cast<int>(canvasSize.x) - legendWidth;
            const int graphHeight = std::min(300, static_cast<int>((availableGraphAreaY - itemSpacingY) / 2.0f));

            // Render graphs only when dimensions are valid
            if (graphHeight > 10 && graphWidth > 10) {
                gpuGraph.RenderTimings(graphWidth, legendWidth, graphHeight, frameOffset, maxFrameTime);
                cpuGraph.RenderTimings(graphWidth, legendWidth, graphHeight, frameOffset, maxFrameTime);
            }

            // Render controls at the bottom if space allows
            if (showControls) {
                ImGui::Separator();
                ImGui::Columns(2, "ProfilerControlsColumns", false);
                ImGui::Checkbox("Stop profiling", &stopProfiling);
                ImGui::Checkbox("Colored legend text", &useColoredLegendText);
                ImGui::DragInt("Frame offset", &frameOffset, 1.0f, 0, 400);
                ImGui::NextColumn();

                ImGui::SliderInt("Frame width", &frameWidth, 1, 4);
                ImGui::SliderInt("Frame spacing", &frameSpacing, 0, 2);
                ImGui::SliderFloat("Transparency", &ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w, 0.0f, 1.0f);
                ImGui::Columns(1);
            }

            if (!stopProfiling) frameOffset = 0;
            gpuGraph.frameWidth = frameWidth;
            gpuGraph.frameSpacing = frameSpacing;
            gpuGraph.useColoredLegendText = useColoredLegendText;
            cpuGraph.frameWidth = frameWidth;
            cpuGraph.frameSpacing = frameSpacing;
            cpuGraph.useColoredLegendText = useColoredLegendText;
        }

        ImGui::End();
    }

    bool stopProfiling;
    int frameOffset;
    ProfilerGraph cpuGraph;
    ProfilerGraph gpuGraph;
    int frameWidth;
    int frameSpacing;
    bool useColoredLegendText;
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    TimePoint prevFpsFrameTime;
    size_t fpsFramesCount;
    float avgFrameTime;
    float maxFrameTime;
};
} // namespace ImGuiUtils