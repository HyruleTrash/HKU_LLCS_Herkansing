//
// Created by Lucas on 29/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_THREADSAFEINVENTORY_H
#define COLLISIONCRISISHERKANSING_THREADSAFEINVENTORY_H
#include "ThreadPool.h"

#include <memory>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <vector>

/**
 * @brief ThreadSafeInventory inventory instance, only meant to be used by ThreadSafeInventory, not outside of this class
 */
struct InventoryItem {
public:
    std::string key = "";
    std::type_index type = std::type_index(typeid(void));
    std::shared_ptr<void> value = nullptr;
};

/**
 * @brief ThreadSafe blackboard, used for inventories
 */
class ConcurrentInventory {
private:
    mutable std::shared_mutex mutex;
    std::vector<InventoryItem> inventory;
    std::unique_ptr<ThreadPool> threadPool;

    /**
     * @brief Checks if item is within inventory, helper used by other functions (assumes lock is already set)
     * @tparam T Type of item to check
     * @param key String as lookup and representation
     * @return true if it exists within inventory, false if not.
     */
    template <typename T> bool hasItemUnsafe(const std::string& key) const {
        const auto type = std::type_index(typeid(T));

        for (auto& item : this->inventory)
            if (item.key == key && item.type == type)
                return true;

        return false;
    }

public:
    // The big 5, because why not
    ConcurrentInventory();
    ConcurrentInventory(size_t capacity, size_t threads);
    ~ConcurrentInventory();
    ConcurrentInventory(const ConcurrentInventory& other);
    ConcurrentInventory& operator=(const ConcurrentInventory& other);
    ConcurrentInventory(ConcurrentInventory&& other);
    ConcurrentInventory& operator=(ConcurrentInventory&& other);

    /**
     * @brief Adds item to inventory
     * @tparam T Type of item to be added
     * @param key String for lookup and representation
     * @param value actual item being added, as type less mem-address
     * @return true if addition succeeded, false if not. will return false if item is already in inventory
     */
    template <typename T>
    std::future<bool> addItem(std::string key, std::shared_ptr<void> value) {
        return this->threadPool->submit([key = std::move(key), value = std::move(value), this] {
            std::unique_lock lock(this->mutex);

            if (hasItemUnsafe<T>(key)) return false; // if item already exists within inventory, then the addition fails

            InventoryItem item;
            item.key = std::move(key);
            item.value = std::move(value);
            item.type = std::type_index(typeid(T));
            this->inventory.push_back(std::move(item));

            // could do another hasItem check here, but being THAT safe seems redundant
            return true;
        });
    }


    /**
     * @brief Gets item from inventory if it exists within the inventory
     * @tparam T Type of item to get
     * @param key String as lookup and representation
     * @return typeless mem-address or null pointer if it doesn't exist
     */
    template <typename T>
    std::future<std::shared_ptr<void>> getItem(const std::string& key) {
        const auto type = std::type_index(typeid(T));
        return this->threadPool->submit([key, this, typeToSearch = std::move(type)] {
            std::shared_lock lock(this->mutex);

            for (const auto& [otherKey, type, value] : this->inventory)
                if (otherKey == key && type == typeToSearch)
                    return value;

            return std::shared_ptr<void>{nullptr};
        });
    }

    /**
     * @brief Removes item to inventory if it exists within the inventory
     * @tparam T Type of item to remove
     * @param key String as lookup and representation
     * @return true if removal succeeded, false if not. will return false if item is not within inventory
     */
    template<typename T>
    std::future<bool> removeItem(const std::string& key) {
        const auto type = std::type_index(typeid(T));
        return this->threadPool->submit([key, this, type = std::move(type)] {
            std::unique_lock lock(this->mutex);

            for (size_t i = 0; i < this->inventory.size(); ++i) {
                if (const auto& item = this->inventory[i]; item.key == key && item.type == type) {
                    this->inventory[i] = std::move(this->inventory.back());
                    this->inventory.pop_back();
                    return true;
                }
            }

            return false;
        });
    }

    /**
     * @brief Checks if item is within inventory
     * @tparam T Type of item to check
     * @param key String as lookup and representation
     * @return true if it exists within inventory, false if not.
     */
    template <typename T>
    std::future<bool> hasItem(const std::string& key) {
        return this->threadPool->submit([this, key] {
            std::shared_lock lock(this->mutex);
            return hasItemUnsafe<T>(key);
        });
    }
};

#endif // COLLISIONCRISISHERKANSING_THREADSAFEINVENTORY_H
