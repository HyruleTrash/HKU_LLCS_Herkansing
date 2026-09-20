//
// Created by Lucas on 11/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_SPATIALHASH_H
#define COLLISIONCRISISHERKANSING_SPATIALHASH_H

#include "entities/Ball.h"

struct Bucket {
public:
    std::tuple<int, int> hash;
    std::vector<Ball*> content;

    Bucket(const std::tuple<int, int>& hash = {0,0});
};

class SpatialHash {
public:
    std::vector<Bucket*> buckets;
    std::vector<Bucket*> edgeBuckets;
    std::tuple<int, int> size;

    SpatialHash(const std::tuple<int, int>& size);
    ~SpatialHash();
    void moveBallIntoBucket(Ball* ball, const sf::Vector2u& windowSize);
    Bucket* findBucket(const std::tuple<int, int>& hash);

    [[nodiscard]] std::tuple<int, int> hash(const float& x, const float& y, const sf::Vector2u& windowSize) const;
};


#endif //COLLISIONCRISISHERKANSING_SPATIALHASH_H