#include "core/Application.h"
#include "core/Profiler.h"
#include "game/Game.h"

int main()
{
    auto* app = new Application();
    app->init();

    auto* profiler = new Profiler(*app->getWindow());
    auto* game = new Game(app->getWindow()->getSize(), profiler);

    app->registerSystem(game);
    app->registerSystem(profiler);

    app->triggerAppLoop();
}
