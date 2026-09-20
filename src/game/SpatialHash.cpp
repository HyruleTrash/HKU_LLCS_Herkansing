//
// Created by Lucas on 11/09/2026.
//

#include "SpatialHash.h"

#include <iostream>

Bucket::Bucket(const std::tuple<int, int>& hash) {
    this->hash = hash;
}

SpatialHash::SpatialHash(const std::tuple<int, int>& size) {
    this->size = size;

    const int cols = std::get<0>(size);
    const int rows = std::get<1>(size);

    std::cout << "Creating buckets " << cols << " " << rows << "\n";
    buckets.reserve(static_cast<std::size_t>(cols) * rows);
    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            auto bucket = new Bucket(std::make_tuple(x, y)); //Allocated memory is leaked
            buckets.push_back(bucket);
            std::cout << x << " " << y << "\n";
            if (x == 0 || x == cols -1 || y == 0 || y == rows - 1) edgeBuckets.push_back(bucket);
        }
    }
    std::cout << "\n";
}

SpatialHash::~SpatialHash() {
    for (const auto b: buckets) {
        delete b;
    }
}

/**
 * Adds ball/item to spatial hash, also automatically updates its position inside of it, if it's already inside of it
 */
void SpatialHash::moveBallIntoBucket(Ball* ball, const sf::Vector2u& windowSize) {
    if (ball == nullptr) return;

    const auto pos = ball->getPosition();
    const auto newHash = hash(std::get<0>(pos), std::get<1>(pos), windowSize);

    // if ball hasn't moved, return
    if (ball->oldBucket != nullptr &&
        std::get<0>(ball->oldBucket->hash) == std::get<0>(newHash) &&
        std::get<1>(ball->oldBucket->hash) == std::get<1>(newHash)
        ) return;

    // get new bucket
    const auto bucket = findBucket(newHash);
    if (bucket == nullptr) return;

    // remove ball from old bucket by swapping with back item and pop
    if (ball->oldBucket != nullptr) {
        auto& oldContent = ball->oldBucket->content; // old content is empty?

        oldContent[ball->oldIndex] = oldContent.back(); // SIGSEGV (Segmentation fault)
        oldContent[ball->oldIndex]->oldIndex = ball->oldIndex;

        oldContent.pop_back();
    }

    // Add ball to new bucket, update references
    bucket->content.push_back(ball);
    ball->oldBucket = bucket;
    ball->oldIndex = bucket->content.size() - 1;
}

Bucket* SpatialHash::findBucket(const std::tuple<int, int>& hash) {
    for (auto item : this->buckets) if (std::get<0>(item->hash) == std::get<0>(hash) && std::get<1>(item->hash) == std::get<1>(hash)) return item;

    buckets.push_back(new Bucket(hash)); // this shouldn't be called since buckets are premade on construction
    return buckets.back();
}

std::tuple<int, int> SpatialHash::hash(const float& x, const float& y, const sf::Vector2u& windowSize) const {
    const float cellWidth = windowSize.x / std::get<0>(size);
    const float cellHeight = windowSize.y / std::get<1>(size);

    int col = static_cast<int>(x / cellWidth);
    int row = static_cast<int>(y / cellHeight);

    col = std::max(0, std::min(col, std::get<0>(size) - 1));
    row = std::max(0, std::min(row, std::get<1>(size) - 1));

    return std::make_tuple(col, row);
}
