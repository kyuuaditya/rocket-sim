#include <iostream>
#include "rocket_data.h"
#include <sfml/Graphics.hpp>

void VisualData::load() {
    // load font for displaying statistics
    if (font.loadFromFile("assets/fonts/HomeVideo-BLG6G.ttf")) { // loading font
        std::cout << "font loaded" << std::endl;
    }
    else {
        std::cout << "font not loaded" << std::endl;
    }
}

void VisualData::initialize() {
    text_rocket_time.setFont(font);
    text_rocket_time.setCharacterSize(30);
    text_rocket_time.setFillColor(sf::Color::White);
    text_rocket_time.setPosition(10, 10); // Position of the text

    text_rocket_velocity.setFont(font);
    text_rocket_velocity.setCharacterSize(30);
    text_rocket_velocity.setFillColor(sf::Color::White);
    text_rocket_velocity.setPosition(10, 40);

    text_rocket_acceleration.setFont(font);
    text_rocket_acceleration.setCharacterSize(30);
    text_rocket_acceleration.setFillColor(sf::Color::White);
    text_rocket_acceleration.setPosition(10, 70); // Position of the text

    text_rocket_fuel.setFont(font);
    text_rocket_fuel.setCharacterSize(30);
    text_rocket_fuel.setFillColor(sf::Color::White);
    text_rocket_fuel.setPosition(10, 100); // Position of the text

    text_rocket_displacement.setFont(font);
    text_rocket_displacement.setCharacterSize(30);
    text_rocket_displacement.setFillColor(sf::Color::White);
    text_rocket_displacement.setPosition(10, 130); // Position of the text

    text_rocket_cfmi.setFont(font);
    text_rocket_cfmi.setCharacterSize(30);
    text_rocket_cfmi.setFillColor(sf::Color::White);
    text_rocket_cfmi.setPosition(10, 160); // Position of the text

    text_rocket_exhaust_velocity.setFont(font);
    text_rocket_exhaust_velocity.setCharacterSize(30);
    text_rocket_exhaust_velocity.setFillColor(sf::Color::White);
    text_rocket_exhaust_velocity.setPosition(10, 190);

    text_simulation_gravity_earth.setFont(font);
    text_simulation_gravity_earth.setCharacterSize(30);
    text_simulation_gravity_earth.setFillColor(sf::Color::White);
    text_simulation_gravity_earth.setPosition(10, 220);

    text_simulation_gravity_moon.setFont(font);
    text_simulation_gravity_moon.setCharacterSize(30);
    text_simulation_gravity_moon.setFillColor(sf::Color::White);
    text_simulation_gravity_moon.setPosition(10, 250);

    text_simulation_phase.setFont(font);
    text_simulation_phase.setCharacterSize(30);
    text_simulation_phase.setFillColor(sf::Color::White);
    text_simulation_phase.setPosition(10, 280);
}

void VisualData::update(const sf::Time& time, double velocity, double acceleration, double fuel, double displacement, double cfmi, double exhaust_velocity, double gravity_earth, double gravity_moon, const std::string& phase) {
    text_rocket_time.setString("Time: " + std::to_string(time.asSeconds()) + " s");
    text_rocket_velocity.setString("Velocity: " + std::to_string(velocity) + " m/s");
    text_rocket_acceleration.setString("Acceleration: " + std::to_string(acceleration) + " m/s^2");
    text_rocket_fuel.setString("Fuel: " + std::to_string(fuel) + " kg");
    text_rocket_displacement.setString("Rocket Distance: " + std::to_string(displacement / 1e3) + " km");
    text_rocket_cfmi.setString("CFMI: " + std::to_string(cfmi));
    text_rocket_exhaust_velocity.setString("Exhaust Velocity: " + std::to_string(exhaust_velocity) + " m/s");
    text_simulation_gravity_earth.setString("Gravity (Earth): " + std::to_string(gravity_earth) + " m/s^2");
    text_simulation_gravity_moon.setString("Gravity (Moon): " + std::to_string(gravity_moon) + " m/s^2");
    text_simulation_phase.setString("Phase: " + phase);
}

void VisualData::draw(sf::RenderWindow& window) {
    window.draw(text_rocket_time);
    window.draw(text_rocket_velocity);
    window.draw(text_rocket_acceleration);
    window.draw(text_rocket_fuel);
    window.draw(text_rocket_displacement);
    window.draw(text_rocket_cfmi);
    window.draw(text_rocket_exhaust_velocity);
    window.draw(text_simulation_gravity_earth);
    window.draw(text_simulation_gravity_moon);
    window.draw(text_simulation_phase);
}