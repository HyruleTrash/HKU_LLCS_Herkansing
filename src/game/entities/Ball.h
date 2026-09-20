//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_BALL_H
#define COLLISIONCRISISHERKANSING_BALL_H
#include "../SpatialHash.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

struct Bucket;

struct Ball {
private:
    std::tuple<float, float> position;
    sf::CircleShape shape;

public:
    Bucket* oldBucket = nullptr;
    size_t oldIndex;

    sf::Vector2f velocity;

    Ball(float x, float y, const float radius, const sf::Color color, float vx, float vy) {
        this->shape.setRadius(radius);
        this->shape.setPosition(sf::Vector2f(x, y));

        this->position = std::make_tuple(x, y);

        this->shape.setFillColor(color);
        this->shape.setOrigin(sf::Vector2f(radius, radius));

        this->velocity = sf::Vector2f(vx, vy);
    }

    void updatePosition(const sf::Vector2f pos) {
        this->shape.setPosition(pos);
        std::get<0>(this->position) = pos.x;
        std::get<1>(this->position) = pos.y;
    }

    void move(const sf::Vector2f offset) {
        const auto x = std::get<0>(this->position) + offset.x;
        const auto y = std::get<1>(this->position) + offset.y;
        updatePosition( sf::Vector2f(x, y));
    }

    std::tuple<float, float> getPosition() {
        return this->position;
    }

    sf::Vector2f getPositionVec2() const {
        return this->shape.getPosition();
    }

    float getRadius() const {
        return this->shape.getRadius();
    }

    void draw(sf::RenderWindow* window) const {
        window->draw(this->shape);
    }
};

#endif //COLLISIONCRISISHERKANSING_BALL_H