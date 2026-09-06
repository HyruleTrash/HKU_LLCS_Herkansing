#include "core/Application.h"
#include "game/Game.h"

int main()
{
    auto* app = new Application();
    app->init();

    auto* game = new Game();
    app->registerSystem(game);

    app->triggerAppLoop();
}
