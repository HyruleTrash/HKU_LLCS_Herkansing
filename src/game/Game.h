//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_GAME_H
#define COLLISIONCRISISHERKANSING_GAME_H
#include "../core/GameSystem.h"
#include "../core/Profiler.h"
#include <SFML/Graphics.hpp>

class Game final : public GameSystem{
private:
    Profiler* profiler;
    sf::CircleShape shape;

public:
    Game(Profiler* profiler);

    std::function<void(AppLoopData*)> registerUpdateFunc() override;
    std::function<void(AppLoopData*)> registerDrawFunc() override;

    void update(const AppLoopData* data);
    void draw(const AppLoopData* data) const;
};

#endif //COLLISIONCRISISHERKANSING_GAME_H