//
// Created by Lucas on 29/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_THREADSAFEINVENTORY_H
#define COLLISIONCRISISHERKANSING_THREADSAFEINVENTORY_H
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
class ThreadSafeInventory {
private:
    mutable std::shared_mutex mutex;
    std::vector<InventoryItem> inventory;

public:
    // The big 5, because why not
    ThreadSafeInventory();
    ThreadSafeInventory(size_t capacity);
    ~ThreadSafeInventory();
    ThreadSafeInventory(const ThreadSafeInventory& other);
    ThreadSafeInventory& operator=(const ThreadSafeInventory& other);
    ThreadSafeInventory(ThreadSafeInventory&& other);
    ThreadSafeInventory& operator=(ThreadSafeInventory&& other);

    /**
     * @brief Adds item to inventory
     * @tparam T Type of item to be added
     * @param key String for lookup and representation
     * @param value actual item being added, as type less mem-address
     * @return true if addition succeeded, false if not. will return false if item is already in inventory
     */
    template <typename T>
    bool addItem(std::string key, std::shared_ptr<void> value);


    /**
     * @brief Gets item from inventory if it exists within the inventory
     * @tparam T Type of item to get
     * @param key String as lookup and representation
     * @return typeless mem-address or null pointer if it doesn't exist
     */
    template<typename T>
    std::shared_ptr<void> getItem(std::string key);

    /**
     * @brief Removes item to inventory if it exists within the inventory
     * @tparam T Type of item to remove
     * @param key String as lookup and representation
     * @return true if removal succeeded, false if not. will return false if item is not within inventory
     */
    template<typename T>
    bool removeItem(std::string key);

    /**
     * @brief Checks if item is within inventory
     * @tparam T Type of item to check
     * @param key String as lookup and representation
     * @return true if it exists within inventory, false if not.
     */
    template<typename T>
    bool hasItem(std::string key);
};

#endif // COLLISIONCRISISHERKANSING_THREADSAFEINVENTORY_H
