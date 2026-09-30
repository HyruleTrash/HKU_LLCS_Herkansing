//
// Created by Lucas on 29/09/2026.
//

#include "ConcurrentInventory.h"

#include <algorithm>
#include <mutex>

#pragma region the big 5
ConcurrentInventory::ConcurrentInventory() : ConcurrentInventory(0, 4) {}
ConcurrentInventory::ConcurrentInventory(const size_t capacity, const size_t threads) {
    this->inventory = {};
    this->inventory.reserve(capacity);
    this->threadPool = std::make_unique<ThreadPool>(threads); // due to here?
    this->threadPool->init();
}

ConcurrentInventory::~ConcurrentInventory() {
    if (this->threadPool) this->threadPool->shutdown();
};

ConcurrentInventory::ConcurrentInventory(const ConcurrentInventory& other) {
    std::shared_lock lock(other.mutex);
    this->inventory = other.inventory;
    this->threadPool = std::make_unique<ThreadPool>(other.threadPool->getThreadCount());
    this->threadPool->init();
}

ConcurrentInventory& ConcurrentInventory::operator=(const ConcurrentInventory& other) {
    if (this == &other) return *this;

    std::unique_lock lock(this->mutex, std::defer_lock);
    std::shared_lock lockOther(other.mutex, std::defer_lock);
    std::lock(lock, lockOther);

    this->inventory = other.inventory;
    this->threadPool = std::make_unique<ThreadPool>(other.threadPool->getThreadCount());
    this->threadPool->init();
    return *this;
}

ConcurrentInventory::ConcurrentInventory(ConcurrentInventory&& other) {
    std::unique_lock lock(other.mutex);
    this->inventory = std::move(other.inventory);
    if (other.threadPool) {
        size_t threads = other.threadPool->getThreadCount();
        other.threadPool->shutdown();
        other.threadPool.reset();

        this->threadPool = std::make_unique<ThreadPool>(threads);
        this->threadPool->init();
    }
}

ConcurrentInventory& ConcurrentInventory::operator=(ConcurrentInventory&& other) {
    if (this == &other) return *this;

    std::unique_lock lock(this->mutex, std::defer_lock);
    std::unique_lock lockOther(other.mutex, std::defer_lock);
    std::lock(lock, lockOther);

    this->inventory = std::move(other.inventory);

    if (this->threadPool) this->threadPool->shutdown();

    if (other.threadPool) {
        size_t threads = other.threadPool->getThreadCount();
        other.threadPool->shutdown();
        other.threadPool.reset();

        this->threadPool = std::make_unique<ThreadPool>(threads);
        this->threadPool->init();
    } else
        this->threadPool.reset();

    return *this;
}
#pragma endregion