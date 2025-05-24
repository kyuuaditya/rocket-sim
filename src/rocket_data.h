#pragma once
#include <sfml/Graphics.hpp>

class VisualData {

public:
    sf::Text text_rocket_time;
    sf::Text text_rocket_velocity;
    sf::Text text_rocket_acceleration;
    sf::Text text_rocket_fuel;
    sf::Text text_rocket_displacement;
    sf::Text text_rocket_cfmi;
    sf::Text text_rocket_exhaust_velocity;
    sf::Text text_simulation_gravity_earth;
    sf::Text text_simulation_gravity_moon;
    sf::Text text_simulation_phase;


private:
    sf::Font font;

public:
    void initialize();
    void load();
    void update(const sf::Time& time, double velocity, double acceleration, double fuel, double displacement, double cfmi, double exhaust_velocity, double gravity_earth, double gravity_moon, const std::string& phase);
    void draw(sf::RenderWindow& window);
};