//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_GAMESYSTEM_H
#define COLLISIONCRISISHERKANSING_GAMESYSTEM_H
#include <functional>

#include "AppLoopData.h"

/**
 * @brief GameSystem acts as an interface for registering something to the main application loop
 */
class GameSystem {
public:
    virtual ~GameSystem() = default;

    /**
     * @brief Used for getting the update functions of the system
     * @return A function that will be triggered each frame
     */
    virtual std::function<void(AppLoopData*)> registerUpdateFunc() = 0;
    virtual std::function<void(AppLoopData*)> registerDrawFunc() = 0;

    /**
     * @brief closes all needed connected running systems
     */
    virtual void stop() = 0;

};

#endif //COLLISIONCRISISHERKANSING_GAMESYSTEM_H