//
// Created by Lucas on 06/09/2026.
//

#include "Game.h"
#include <iostream>

template <typename Clock, typename Duration>
float SecondsSince(std::chrono::time_point<Clock, Duration> start)
{
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();
}

Game::Game(Profiler* profiler) {
    this->profiler = profiler;

    this->shape = sf::CircleShape(100.f);
    this->shape.setFillColor(sf::Color::Green);

    this->inventory = ConcurrentInventory();
}

std::function<void(AppLoopData*)> Game::registerUpdateFunc() {
    std::cout << "Registering main game update loop to app loop.\n";
    return std::bind(&Game::update, this, std::placeholders::_1);
}

std::function<void(AppLoopData *)> Game::registerDrawFunc() {
    std::cout << "Registering main game draw call to app.\n";
    return std::bind(&Game::draw, this, std::placeholders::_1);
}

void Game::updateScreenSaver(const AppLoopData* data) {
    constexpr float speed = 1;
    constexpr float distance = 100;
    const auto windowSize = data->window->getSize();
    const auto shapeSize = this->shape.getGlobalBounds().size;
    this->shape.setPosition({
        static_cast<float>(windowSize.x / 2) - shapeSize.x / 2,
        static_cast<float>(windowSize.y / 2) - shapeSize.y / 2,
    });
    this->shape.move({0, static_cast<float>(sin(SecondsSince(data->startApplication) * speed) * distance)});
}

void Game::doTestOne() {
    std::cout << "Test 1: 20 users try to take Excalibur at the exact same time\n";

    this->inventory.addItem<int>("Excalibur", std::make_shared<int>(9999)).get();

    std::atomic<int> claimCount{0};
    constexpr int userCount = 20;
    std::vector<std::future<void>> userThreads;

    // create jobs
    for (int i = 0; i < userCount; ++i) {
        userThreads.push_back(std::async(std::launch::async, [&]() {
            if (this->inventory.removeItem<int>("Excalibur").get())
                ++claimCount;
        }));
    }

    // do all jobs
    for (auto& f : userThreads) f.wait();

    if (claimCount == 1 && !this->inventory.hasItem<int>("Excalibur").get())
        std::cout << "PASS: Exactly 1 user claimed Excalibur (Others received false upon retrieval)\n";
    else
        std::cout << "FAIL: Race Condition met, Excalibur was claimed '" << claimCount << "' times\n";
}

void Game::doTestTwo() {
    std::cout << "\nTest 2: 100 goblins stashing unique items at the same time...\n";

    constexpr int totalItemsToAdd = 100;
    std::atomic<int> addSuccesses{0};
    std::vector<std::future<void>> entityThreads;

    for (int i = 0; i < totalItemsToAdd; ++i) {
        entityThreads.push_back(std::async(std::launch::async, [&, i]() {
            const std::string key = "Gold_Pouch_" + std::to_string(i);
            if (this->inventory.addItem<int>(key, std::make_shared<int>(50)).get())
                ++addSuccesses;
        }));
    }

    for (auto& f : entityThreads) f.wait();

    std::atomic<int> readSuccesses{0};
    std::vector<std::future<void>> entityThreads2;

    for (int i = 0; i < totalItemsToAdd; ++i) {
        entityThreads2.push_back(std::async(std::launch::async, [&, i]() {
            const std::string key = "Gold_Pouch_" + std::to_string(i);
            if (this->inventory.hasItem<int>(key).get())
                ++readSuccesses;
        }));
    }

    for (auto& f : entityThreads2) f.wait();

    if (addSuccesses == totalItemsToAdd && readSuccesses == totalItemsToAdd)
        std::cout << "PASS: All 100 pouches safely added and read.\n";
    else
        std::cout << "FAIL: Added: " << addSuccesses << " | Readable: " << readSuccesses << "\n";
}

void Game::doTestThree() {
    std::cout << "\nTest 3: Two users add 'Dragon_Egg' at the same time\n";

    std::atomic<int> eggCount{0};
    std::vector<std::future<void>> mageThreads;

    for (int i = 0; i < 2; ++i) {
        mageThreads.push_back(std::async(std::launch::async, [&]() {
            if (this->inventory.addItem<int>("Dragon_Egg", std::make_shared<int>(100)).get())
                ++eggCount;
        }));
    }

    for (auto& f : mageThreads) f.wait();

    if (eggCount == 1) std::cout << "PASS: Duplicate key rejected\n";
    else std::cout << "FAIL: Duplicate key(s) accepted Count: " << eggCount << "\n";
}

void Game::doTestFour() {
    std::cout << "\nTest 4: Read/Write stress test\n";

    std::atomic<bool> usersActive{true};
    std::atomic<int> writeCount{0};
    std::atomic<int> readCount{0};

    const auto fakeUser = std::async(std::launch::async, [&]() {
        while (usersActive) {
            this->inventory.addItem<int>("Health_Potion", std::make_shared<int>(100)).get();
            this->inventory.removeItem<int>("Health_Potion").get();
            ++writeCount;
        }
    });

    std::vector<std::future<void>> fakeUsers;
    for (int i = 0; i < 4; ++i) {
        fakeUsers.push_back(std::async(std::launch::async, [&]() {
            while (usersActive) {
                this->inventory.hasItem<int>("Health_Potion").get();
                ++readCount;
            }
        }));
    }

    // execution window
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    usersActive = false;

    fakeUser.wait();
    for (auto& f : fakeUsers) f.wait();

    std::cout << "PASS: \n Survived: \n  " << writeCount << " Writes\n  " << readCount << " Reads \nWithout a crash or deadlock encounter\n\n";
}

void Game::update(const AppLoopData* data) {
    updateScreenSaver(data);

    static bool testsDone = false;
    if (testsDone) return;
    testsDone = true;

    std::cout << "\nSTARTING THREADSAFE INVENTORY TEST\n";
    std::cout << "-----------------------------------------------\n\n";

    doTestOne();
    doTestTwo();
    doTestThree();
    doTestFour();

    std::cout << "-----------------------------------------------\n";
    std::cout << "ALL THREAD SAFETY TESTS COMPLETED\n";
}

void Game::draw(const AppLoopData *data) const { data->window->draw(shape); }
