//
// Created by Lucas on 29/09/2026.
//

#include "ThreadSafeInventory.h"

#include <mutex>

#pragma region the big 5
ThreadSafeInventory::ThreadSafeInventory() : ThreadSafeInventory(0) {}
ThreadSafeInventory::ThreadSafeInventory(const size_t capacity) {
    this->inventory = {};
    this->inventory.reserve(capacity);
}

ThreadSafeInventory::~ThreadSafeInventory() = default;

ThreadSafeInventory::ThreadSafeInventory(const ThreadSafeInventory& other) {
    std::shared_lock lock(other.mutex);
    inventory = other.inventory;
}

ThreadSafeInventory& ThreadSafeInventory::operator=(const ThreadSafeInventory& other) {
    if (this == &other) return *this;

    std::unique_lock lock(mutex, std::defer_lock);
    std::shared_lock lockOther(other.mutex, std::defer_lock);
    std::lock(lock, lockOther);

    inventory = other.inventory;
    return *this;
}

ThreadSafeInventory::ThreadSafeInventory(ThreadSafeInventory&& other) {
    std::unique_lock lock(other.mutex);
    inventory = std::move(other.inventory);
}

ThreadSafeInventory& ThreadSafeInventory::operator=(ThreadSafeInventory&& other) {
    if (this == &other) return *this;

    std::unique_lock lock(mutex, std::defer_lock);
    std::unique_lock lockOther(other.mutex, std::defer_lock);
    std::lock(lock, lockOther);

    inventory = std::move(other.inventory);
    return *this;
}
#pragma endregion

template <typename T>
bool ThreadSafeInventory::addItem(std::string key, std::shared_ptr<void> value) {

}

template <typename T>
std::shared_ptr<void> ThreadSafeInventory::getItem(std::string key) {

}

template <typename T>
bool ThreadSafeInventory::removeItem(std::string key) {

}

template <typename T>
bool ThreadSafeInventory::hasItem(std::string key) {

}