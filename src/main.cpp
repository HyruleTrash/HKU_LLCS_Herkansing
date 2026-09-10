#include "core/Application.h"
#include "core/Profiler.h"
#include "game/Game.h"

int main()
{
    auto* app = new Application();
    app->init();

    auto* game = new Game();
    app->registerSystem(game);

    auto* profiler = new Profiler(*app->getWindow());
    app->registerSystem(profiler);

    app->triggerAppLoop();
}
