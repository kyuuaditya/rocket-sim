#pragma once
#include "constants.h"
#include <SFML/Graphics.hpp>

class Graphics {
public:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite; // Create a sprite for the background
    sf::Texture moonTexture;
    sf::Sprite moonSprite;
    sf::Texture rocketTexture;

    sf::RectangleShape rocket; // Rocket representation

public:
    void load(int phase);
    void initialize(sf::RenderWindow& window); // takes in render window
    void update(sf::RenderWindow& window, long double& netDisplacement, int phase, const long double& GapEarthMoon, long double totalDistanceCovered = 0);
    void draw(sf::RenderWindow& window, int phase);
};