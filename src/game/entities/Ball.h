//
// Created by Lucas on 06/09/2026.
//

#ifndef COLLISIONCRISISHERKANSING_BALL_H
#define COLLISIONCRISISHERKANSING_BALL_H
#include "SFML/Graphics/CircleShape.hpp"

struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity;

    Ball(float x, float y, float radius, sf::Color color, float vx, float vy) {
        shape.setRadius(radius);
        shape.setPosition(sf::Vector2f(x, y));
        shape.setFillColor(color);
        shape.setOrigin(sf::Vector2f(radius, radius)); // Center origin
        velocity = sf::Vector2f(vx, vy);
    }
};

#endif //COLLISIONCRISISHERKANSING_BALL_H