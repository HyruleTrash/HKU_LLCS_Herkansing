//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_GAME_H
#define COLLISIONCRISISHERKANSING_GAME_H
#include "../core/ConcurrentInventory.h"
#include "../core/GameSystem.h"
#include "../core/Profiler.h"
#include <SFML/Graphics.hpp>

class Game final : public GameSystem{
private:
    ConcurrentInventory inventory;
    sf::CircleShape shape;
    Profiler* profiler;

  public:
    Game(Profiler* profiler);

    std::function<void(AppLoopData*)> registerUpdateFunc() override;
    std::function<void(AppLoopData*)> registerDrawFunc() override;
    void updateScreenSaver(const AppLoopData* data);
    void doTestOne();
    void doTestTwo();
    void doTestThree();
    void doTestFour();

    void update(const AppLoopData* data);
    void draw(const AppLoopData* data) const;
};

#endif //COLLISIONCRISISHERKANSING_GAME_H