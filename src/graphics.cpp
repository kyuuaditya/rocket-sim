#include "graphics.h"
#include "constants.h"
#include <iostream>
#include <sfml/Graphics.hpp>

void Graphics::load(int phase) {
    if (phase == 0) {
        // Load background texture
        if (!backgroundTexture.loadFromFile("assets/background/9999c.png")) { // Replace with your image path
            std::cerr << "Failed to load background image." << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        if (!moonTexture.loadFromFile("assets/texture/moon_overlay.png")) { // Replace with your image path
            std::cerr << "Failed to load moon image." << std::endl;
        }
        moonSprite.setTexture(moonTexture);


        if (!rocketTexture.loadFromFile("assets/texture/rocket.png")) { // Replace with your rocket image path
            std::cerr << "Failed to load rocket image" << std::endl;
        }
    }
    else if (phase == 1) {
        // Load background texture
        if (!backgroundTexture.loadFromFile("assets/background/123.png")) { // Replace with your image path
            std::cerr << "Failed to load background image." << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        // update rocket sprite
        if (!rocketTexture.loadFromFile("assets/texture/rocket_noflame.png")) { // Replace with your rocket image path
            std::cerr << "Failed to load rocket image" << std::endl;
        }
        rocket.setTexture(&rocketTexture);
    }
    else if (phase == 2) {
        // update rocket sprite
        if (!rocketTexture.loadFromFile("assets/texture/rocket.png")) { // Replace with your rocket image path
            std::cerr << "Failed to load rocket image" << std::endl;
        }

        // Set the texture for the rocket sprite
        rocket.setTexture(&rocketTexture);
    }
    else if (phase == 3) {
        // update rocket sprite
        if (!rocketTexture.loadFromFile("assets/texture/rocket_noflame.png")) { // Replace with your rocket image path
            std::cerr << "Failed to load rocket image" << std::endl;
        }

        // Set the texture for the rocket sprite
        rocket.setTexture(&rocketTexture);
    }
}

void Graphics::initialize(sf::RenderWindow& window) {
    rocket.setSize(sf::Vector2f(64, 64)); // Rocket representation
    rocket.setTexture(&rocketTexture);

    rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, window.getSize().y - rocket.getSize().y); // Initial position of the rocket
}

void Graphics::update(sf::RenderWindow& window, long double& netDisplacement, int phase, const long double& GapEarthMoon, long double totalDistanceCovered) {
    // Update the rocket's position based on its current state
    if (phase == 0) { // Phase 0: Lift-off
        rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * (1 - (netDisplacement / 2200000))); // Initial position of the rocket
    }
    else if (phase == 1) { // Phase 1: Cruising
        rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * (1 - (netDisplacement / GapEarthMoon))); // Initial position of the rocket
    }
    else {
        rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * ((netDisplacement - totalDistanceCovered) / (GapEarthMoon - totalDistanceCovered))); // Initial position of the rocket
    }
}

void Graphics::draw(sf::RenderWindow& window, int phase) {
    if (phase == 0 || phase == 1 || phase == 2) { // Phase 0: Lift-off
        window.draw(backgroundSprite); // Draw the background
        window.draw(rocket); // Draw the rocket        
    }
    else {
        window.draw(backgroundSprite); // Draw the background
        window.draw(moonSprite); // Draw the moon overlay
        window.draw(rocket);
    }
}