#include <iostream>
#include <fstream>
#include <cmath>
#include <sfml/Graphics.hpp>

bool visual = false; // Set to true for visual simulation,
bool write = false; // set to true to write to CSV
bool write_liftoff = true;
double target_velocity = 11000;
double braking_distance_s3 = 2000 * 1e3; // 2000 km

// Function to write data to a CSV file
std::ofstream file("filename.csv");
void writeDataToCSV(double a, double b, double c) {
    if (write || write_liftoff) {
        if (file.is_open()) {
            file << a << " " << b << " " << c << "\n";
        }
        else {
            std::cerr << "Failed to open file" << std::endl;
        }
    }
}

int main() {
    // data of planetary bodies
    const long double massEarth = 5.972e24; // kg
    const long double radiusEarth = 6.378e6; // meters

    const long double massSun = 1.989e30; // kg
    const long double radiusSun = 6.963e8; // meters

    const long double massMoon = 7.347e22; // kg
    const long double radiusMoon = 1.737e6; // meters

    const long double d = 3.844006e8 - radiusEarth - radiusMoon; // Distance between Earth & Moon minus their radii

    const long double S = 1.496e11; // distance from earth to sun (centres)

    // constants
    const long double G = 6.674e-11; // gravitational constant

    // rocket data
    const long double enginePower = 50.0e10; // watts
    long double rocketTotalMass = 2.8e6; // kg
    long double rocketDryMass = 4.0e5; // kg
    long double initialFuelMass = rocketTotalMass - rocketDryMass;
    long double currentFuelMass = initialFuelMass;
    long double currentRocketMass = currentFuelMass + rocketDryMass;

    long double lowerLimitEFMI = 2;
    long double higherLimitEFMI = 20000;
    long double currentEFMI = 20000; // Initial fuel burn rate

    // initialize rocket statistics
    long double x = 0; // displacement initially set to 0
    long double rocketVelocity = 0;
    long double acceleration = 0;
    long double timeExpended_1 = 0;
    long double exhaustVelocity = 0;
    long double thrust = 0;
    long double netForce = 0;

    // time step for simulation
    long double timeStep = 0.001; // in seconds

    // create the window for the simulation
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "simulation", sf::Style::Fullscreen);

    // Load background texture
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/background/9999c.png")) { // Replace with your image path
        std::cerr << "Failed to load background image." << std::endl;
        return -1;
    }

    // Create a sprite for the background
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    // Load moon overlay texture
    sf::Texture moonTexture;
    if (!moonTexture.loadFromFile("assets/texture/moon_overlay.png")) { // Replace with your image path
        std::cerr << "Failed to load moon image." << std::endl;
        return -1;
    }

    // Create a sprite for the background
    sf::Sprite moonSprite;
    moonSprite.setTexture(moonTexture);

    // rocket representation
    sf::RectangleShape rocket(sf::Vector2f(64, 64)); // Rocket representation

    // Load rocket texture
    sf::Texture rocketTexture;
    if (!rocketTexture.loadFromFile("assets/texture/rocket.png")) { // Replace with your rocket image path
        std::cerr << "Failed to load rocket image" << std::endl;
        return -1;
    }

    // Set the texture for the rocket sprite
    rocket.setTexture(&rocketTexture);
    rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, window.getSize().y - rocket.getSize().y); // Initial position of the rocket

    // load font for displaying statistics
    sf::Font font;
    if (font.loadFromFile("assets/fonts/HomeVideo-BLG6G.ttf")) { // loading font
        std::cout << "font loaded" << std::endl;
    }
    else {
        std::cout << "font not loaded" << std::endl;
    }

    // statistic text sfml initialization
    sf::Text text_rocket_time;
    sf::Text text_rocket_velocity;
    sf::Text text_rocket_acceleration;
    sf::Text text_rocket_fuel;
    sf::Text text_rocket_displacement;
    sf::Text text_rocket_cfmi;
    sf::Text text_rocket_exhaust_velocity;
    sf::Text text_simulation_gravity_earth;
    sf::Text text_simulaiton_graivty_moon;
    sf::Text text_simulation_phase;

    text_rocket_time.setFont(font);
    text_rocket_time.setCharacterSize(36);
    text_rocket_time.setFillColor(sf::Color::White);
    text_rocket_time.setPosition(10, 10); // Position of the text

    text_rocket_velocity.setFont(font);
    text_rocket_velocity.setCharacterSize(36);
    text_rocket_velocity.setFillColor(sf::Color::White);
    text_rocket_velocity.setPosition(10, 40);

    text_rocket_acceleration.setFont(font);
    text_rocket_acceleration.setCharacterSize(36);
    text_rocket_acceleration.setFillColor(sf::Color::White);
    text_rocket_acceleration.setPosition(10, 70); // Position of the text

    text_rocket_fuel.setFont(font);
    text_rocket_fuel.setCharacterSize(36);
    text_rocket_fuel.setFillColor(sf::Color::White);
    text_rocket_fuel.setPosition(10, 100); // Position of the text

    text_rocket_displacement.setFont(font);
    text_rocket_displacement.setCharacterSize(36);
    text_rocket_displacement.setFillColor(sf::Color::White);
    text_rocket_displacement.setPosition(10, 130); // Position of the text

    text_rocket_cfmi.setFont(font);
    text_rocket_cfmi.setCharacterSize(36);
    text_rocket_cfmi.setFillColor(sf::Color::White);
    text_rocket_cfmi.setPosition(10, 160); // Position of the text

    text_rocket_exhaust_velocity.setFont(font);
    text_rocket_exhaust_velocity.setCharacterSize(36);
    text_rocket_exhaust_velocity.setFillColor(sf::Color::White);
    text_rocket_exhaust_velocity.setPosition(10, 190);

    text_simulation_gravity_earth.setFont(font);
    text_simulation_gravity_earth.setCharacterSize(36);
    text_simulation_gravity_earth.setFillColor(sf::Color::White);
    text_simulation_gravity_earth.setPosition(10, 220);

    text_simulaiton_graivty_moon.setFont(font);
    text_simulaiton_graivty_moon.setCharacterSize(36);
    text_simulaiton_graivty_moon.setFillColor(sf::Color::White);
    text_simulaiton_graivty_moon.setPosition(10, 250);

    text_simulation_phase.setFont(font);
    text_simulation_phase.setCharacterSize(36);
    text_simulation_phase.setFillColor(sf::Color::White);
    text_simulation_phase.setPosition(10, 280);


    int count = 0; // counter for simulation steps
    double work = 0; // cost of the simulation
    double timeEfficiencyConstant = 1; // time efficiency of the simulation
    double fuelEfficiencyConstant = 1; // fuel efficiency of the simulation
    double time_cost_1 = 0;

    // -------------------------------------------------------------- State 1 ----------------------------------------------------------
    while (rocketVelocity < target_velocity) { // Run for 5 minutes
        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // check if fuel is available
        if (currentFuelMass > 0) {
            // calculating thrust
            thrust = sqrt(2 * enginePower * currentEFMI);

            // Burn fuel
            currentFuelMass -= currentEFMI * timeStep;
            if (currentFuelMass < 0) {
                break;
                currentFuelMass += currentEFMI * timeStep;
                currentEFMI = 0; // Stop engine if fuel runs out
                thrust = 0;
            }

            // Update rocket mass
            currentRocketMass = currentFuelMass + rocketDryMass;
        }

        // Correct net force calculation
        netForce = thrust
            - ((G * massEarth * currentRocketMass) / pow((radiusEarth + x), 2))
            + ((G * massMoon * currentRocketMass) / pow((radiusMoon + d - x), 2));

        // Acceleration update
        acceleration = netForce / currentRocketMass;
        // if acceleration is too high reduce fuel burn rate
        if (acceleration >= 50 && currentEFMI > 1) {
            currentEFMI -= 1;
        }

        // update rocket velocity and position
        long double distanceInTimeStep = rocketVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        work += distanceInTimeStep * netForce;
        rocketVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        timeExpended_1 += 1;
        time_cost_1 += timeStep / distanceInTimeStep;

        if (count % 1000 == 0 && (write || write_liftoff)) {
            writeDataToCSV(count / 1000, x, rocketVelocity);
        }

        if (visual) {
            if (count % 20 == 0 && window.isOpen()) { // Update every 20 instances for smoother animation
                window.clear();

                // Update
                rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * (1 - (x / 2200000))); // Initial position of the rocket
                text_rocket_velocity.setString("Rocket Speed: " + std::to_string(rocketVelocity) + " m/s");
                text_rocket_acceleration.setString("Rocket Acceleration: " + std::to_string(acceleration) + " m/s^2");
                text_rocket_fuel.setString("Rocket Fuel: " + std::to_string(currentFuelMass) + " kg");
                text_rocket_time.setString("Rocket Time: " + std::to_string(timeExpended_1 * timeStep) + " seconds");
                text_rocket_displacement.setString("Rocket Distance: " + std::to_string(x / 1e3) + " km");
                text_rocket_cfmi.setString("Current Fuel Mass Input: " + std::to_string(currentEFMI));
                text_simulation_phase.setString("Liftoff!");

                // draw
                window.draw(backgroundSprite); // Draw the background
                window.draw(text_simulation_phase); // Draw the text_simulation_phase text
                window.draw(text_rocket_velocity); // Draw the rocket speed text
                window.draw(text_rocket_acceleration); // Draw the rocket acceleration text
                window.draw(text_rocket_fuel); // Draw the rocket fuel text
                window.draw(text_rocket_time); // Draw the rocket time text
                window.draw(text_rocket_displacement); // Draw the rocket distance text
                window.draw(text_rocket_cfmi);
                window.draw(rocket);
                // sf::sleep(sf::milliseconds(1)); // Sleep for 1 millisecond to control the frame rate


                window.display();
            }
        }

        count++;
    }
    // normalize timeExpended to seconds
    timeExpended_1 *= timeStep;
    time_cost_1 /= 1e3; // normalize time cost to seconds/km
    time_cost_1 += 1e3 / sqrt(rocketVelocity);
    double fuel_cost_1 = (initialFuelMass - currentFuelMass) / work;
    double cost_liftoff = time_cost_1 * timeEfficiencyConstant + fuel_cost_1 * fuelEfficiencyConstant + time_cost_1 * fuel_cost_1; // cost of the simulation

    std::cout << "------------------------------------------------Stage 1 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << timeExpended_1 << " seconds\n";
    std::cout << "Final velocity: " << rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << x / 1e3 << " km\n";
    std::cout << "Distance Left: " << (d - x) / 1e3 << "km" << std::endl;
    std::cout << "Current Rocket Mass: " << currentRocketMass << " kg\n";
    std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";
    std::cout << "Fuel Cost: " << fuel_cost_1 << " Kg/j\n";
    std::cout << "Time Cost: " << time_cost_1 << " s/km\n";
    std::cout << "Cost of the simulation: " << cost_liftoff << std::endl;

    // -------------------------------------------------------------- State 2 ----------------------------------------------------------
    // Load background texture
    if (!backgroundTexture.loadFromFile("assets/background/123.png")) { // Replace with your image path
        std::cerr << "Failed to load background image." << std::endl;
        return -1;
    }

    // update the sprite for the background
    backgroundSprite.setTexture(backgroundTexture);

    // update rocket sprite
    if (!rocketTexture.loadFromFile("assets/texture/rocket_noflame.png")) { // Replace with your rocket image path
        std::cerr << "Failed to load rocket image" << std::endl;
        return -1;
    }

    // Set the texture for the rocket sprite
    rocket.setTexture(&rocketTexture);

    // initialize rocket statistics
    double cruiseVelocity = rocketVelocity;
    double distanceToCruise = d - x; // Distance to cruise
    double distanceCruised = 0;

    count = 0;
    double timeExpended_2 = 0;
    double time_cost_2 = 0;
    bool rocketStatus = true; // Rocket status

    timeStep = 1; // in seconds
    while (distanceCruised + braking_distance_s3 < distanceToCruise && cruiseVelocity > 0) { // 30 hours for reference
        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // calculating effects of gravity
        netForce =
            ((G * massMoon * currentRocketMass) / pow((radiusMoon + d - x), 2))
            - ((G * massEarth * currentRocketMass) / pow((radiusEarth + x), 2));

        acceleration = netForce / currentRocketMass;

        // update rocket velocity and position
        long double distanceInTimeStep = cruiseVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        cruiseVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        distanceCruised += distanceInTimeStep;
        timeExpended_2 += 1;

        time_cost_2 += timeStep / distanceInTimeStep; // time cost for the simulation

        if (write) {
            writeDataToCSV(count, x, cruiseVelocity);
        }

        if (visual) {
            // Update rocket position for visualization
            if (window.isOpen()) {
                window.clear();

                rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * (1 - (x / d))); // Initial position of the rocket
                text_rocket_velocity.setString("Rocket Speed: " + std::to_string(cruiseVelocity) + " m/s");
                text_rocket_acceleration.setString("Rocket Acceleration: " + std::to_string(acceleration) + " m/s^2");
                text_rocket_fuel.setString("Rocket Fuel: " + std::to_string(currentFuelMass) + " kg");
                text_rocket_time.setString("Rocket Time: " + std::to_string(timeExpended_2 * timeStep) + " seconds");
                text_rocket_displacement.setString("Rocket Distance: " + std::to_string(x / 1e3) + " km");
                text_simulation_phase.setString("Cruiseing!");

                window.draw(backgroundSprite); // Draw the background
                window.draw(text_simulation_phase); // Draw the text_simulation_phase text
                window.draw(text_rocket_velocity); // Draw the rocket speed text
                window.draw(text_rocket_acceleration); // Draw the rocket acceleration text
                window.draw(text_rocket_fuel); // Draw the rocket fuel text
                window.draw(text_rocket_time); // Draw the rocket time text
                window.draw(text_rocket_displacement); // Draw the rocket distance text
                window.draw(rocket);
                // sf::sleep(sf::milliseconds(1)); // Sleep for 1 millisecond to control the frame rate

                window.display();
            }
        }

        count++;
    }

    if (cruiseVelocity < 0) {
        std::cout << "Rocket has stopped moving" << std::endl;
        currentEFMI = 0; // Stop engine if fuel runs out
        thrust = 0;
        rocketStatus = false; // Stop the rocket
    }


    std::cout << "------------------------------------------------Stage 2 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << timeExpended_2 << " seconds\n";
    std::cout << "Cruise velocity: " << cruiseVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceCruised / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (x) / 1e3 << " km\n";
    std::cout << "Distance Left: " << (d - x) / 1e3 << "km" << std::endl;
    std::cout << "time cost: " << time_cost_2 << " s/km\n";
    double cost_cruise = time_cost_2;

    // -------------------------------------------------------------- State 3 ----------------------------------------------------------
    // update rocket sprite
    if (!rocketTexture.loadFromFile("assets/texture/rocket.png")) { // Replace with your rocket image path
        std::cerr << "Failed to load rocket image" << std::endl;
        return -1;
    }

    // Set the texture for the rocket sprite
    rocket.setTexture(&rocketTexture);

    // initialize rocket statistics
    rocketVelocity = cruiseVelocity; // Reset velocity to cruise velocity
    double distancetoDecelerate = d - x; // Distance to decelerate
    double totalDistanceCovered = x;
    double distanceDecelerated = 0;
    thrust = 0;
    netForce = 0;
    currentEFMI = 0;
    double rocketMassPostCruise = currentFuelMass;

    double timeExpended_3 = 0;

    count = 0;
    work = 0;
    double time_cost_3 = 0; // time cost for the simulation
    double fuel_cost_3 = 0; // fuel cost for the simulation

    timeStep = 0.001; // in seconds

    while (distancetoDecelerate - 1 * 1e3 > distanceDecelerated && rocketVelocity > 0) {
        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        if (currentFuelMass > 0) {
            // calculating thrust
            thrust = -sqrt(2 * enginePower * currentEFMI); // Negative thrust for braking

            // Burn fuel
            currentFuelMass -= currentEFMI * timeStep;
            if (currentFuelMass < 0) {
                break;
                currentFuelMass += currentEFMI * timeStep;
                currentEFMI = 0; // Stop engine if fuel runs out
                thrust = 0;
            }

            // Update rocket mass
            currentRocketMass = currentFuelMass + rocketDryMass;
        }

        // Net force calculation
        netForce = thrust
            - ((G * massEarth * currentRocketMass) / pow((radiusEarth + x), 2))
            + ((G * massMoon * currentRocketMass) / pow((radiusMoon + d - x), 2));

        acceleration = netForce / currentRocketMass;

        // Update rocket velocity and position
        long double distanceInTimeStep = rocketVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        rocketVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        distanceDecelerated += distanceInTimeStep;
        timeExpended_3 += 1;
        work += -distanceInTimeStep * netForce;
        time_cost_3 += timeStep / distanceInTimeStep; // time cost for the simulation

        // change efmi as per goal final velocity ie. 100 m/s
        if ((-(pow(rocketVelocity, 2) - pow(100, 2)) / (2 * (distancetoDecelerate - 1 * 1e3 - distanceDecelerated)) < acceleration) && acceleration > -50) {
            currentEFMI += 1;
        }
        else {
            if (currentEFMI >= 1) currentEFMI -= 1;
        }
        if (acceleration < -50 && currentEFMI >= 1) currentEFMI -= 1; // safety check for 5g


        if (count % 1000 == 0 && write) {
            writeDataToCSV(count / 1000, x, rocketVelocity);
        }

        if (count % 10 == 0) {
            if (visual) {
                window.clear();

                rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * ((x - totalDistanceCovered) / (d - totalDistanceCovered))); // Initial position of the rocket
                text_rocket_velocity.setString("Rocket Speed: " + std::to_string(rocketVelocity) + " m/s");
                text_rocket_acceleration.setString("Rocket Acceleration: " + std::to_string(acceleration) + " m/s^2");
                text_rocket_fuel.setString("Rocket Fuel: " + std::to_string(currentFuelMass) + " kg");
                text_rocket_time.setString("Rocket Time: " + std::to_string(timeExpended_3 * timeStep) + " seconds");
                text_rocket_displacement.setString("Rocket Distance: " + std::to_string(x / 1e3) + " km");
                text_rocket_cfmi.setString("Current Fuel Mass Input: " + std::to_string(currentEFMI));
                text_simulation_phase.setString("decelerating!");

                window.draw(backgroundSprite); // Draw the background
                window.draw(text_simulation_phase); // Draw the text_simulation_phase text
                window.draw(text_rocket_velocity); // Draw the rocket speed text
                window.draw(text_rocket_acceleration); // Draw the rocket acceleration text
                window.draw(text_rocket_fuel); // Draw the rocket fuel text
                window.draw(text_rocket_time); // Draw the rocket time text
                window.draw(text_rocket_displacement); // Draw the rocket distance text
                window.draw(text_rocket_cfmi);
                window.draw(rocket);
                // sf::sleep(sf::milliseconds(1)); // Sleep for 1 millisecond to control the frame rate


                window.display();
            }
        }

        count++;
    }
    if (rocketVelocity < 0) {
        time_cost_3 = 1e3;
        fuel_cost_3 = 1e3;
    }
    else {
        time_cost_3 /= 1e3; // normalize time cost to seconds/km
        time_cost_3 += rocketVelocity / 100; // time cost for the simulation
        fuel_cost_3 += (rocketMassPostCruise - currentFuelMass) / work; // fuel cost for the simulation
    }
    timeExpended_3 *= timeStep;
    double cost_decelerate = time_cost_3 * timeEfficiencyConstant + fuel_cost_3 * fuelEfficiencyConstant + time_cost_3 * fuel_cost_3; // cost of the simulation


    std::cout << "------------------------------------------------Stage 3 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << timeExpended_3 << " seconds\n";
    std::cout << "Cruise velocity: " << rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (x) / 1e3 << " km\n";
    std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";
    std::cout << "Distance Left: " << (d - x) << "m\n";
    std::cout << "time cost: " << time_cost_3 << " s/km\n";
    std::cout << "Fuel Cost: " << fuel_cost_3 << " Kg/j\n";
    std::cout << "Cost of the simulation: " << cost_decelerate << std::endl;
    // -------------------------------------------------------------- State 4 ----------------------------------------------------------
    // initialize rocket statistics
    distancetoDecelerate = d - x;
    totalDistanceCovered = x;
    currentEFMI = 0;
    thrust = 0;
    netForce = 0;
    distanceDecelerated = 0;
    double landing_range = 1; // 1 meter
    bool landing_phrase = true;
    double time_cost_4 = 0; // time cost for the simulation
    double fuel_cost_4 = 0; // fuel cost for the simulation
    double rocketMassPostDecelerating = currentFuelMass;

    count = 0;
    double timeExpended_4 = 0;

    timeStep = 0.001; // in seconds

    while (distancetoDecelerate - landing_range > distanceDecelerated && x < d && landing_phrase && rocketStatus) {
        if (count * timeStep > 100) { // 100 seconds allowed for landing
            break;
        }

        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        if (currentFuelMass > 0) {
            thrust = -sqrt(2 * enginePower * currentEFMI); // Negative thrust for braking

            // Burn fuel
            currentFuelMass -= currentEFMI * timeStep;
            if (currentFuelMass < 0) {
                std::cout << "Ran out of fuel when landing" << std::endl;
                currentFuelMass += currentEFMI * timeStep;
                currentEFMI = 0; // Stop engine if fuel runs out
                thrust = 0;
                break;
            }

            // Update rocket mass
            currentRocketMass = currentFuelMass + rocketDryMass;
        }

        // Correct net force calculation
        netForce = thrust
            - ((G * massEarth * currentRocketMass) / pow((radiusEarth + x), 2))
            + ((G * massMoon * currentRocketMass) / pow((radiusMoon + d - x), 2));

        // Acceleration update
        acceleration = netForce / currentRocketMass;

        // update rocket velocity and position
        long double distanceInTimeStep = rocketVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        rocketVelocity += acceleration * timeStep;
        x += distanceInTimeStep;

        distanceDecelerated += distanceInTimeStep;
        timeExpended_4 += 1;
        work += -distanceInTimeStep * netForce;
        time_cost_4 += timeStep / distanceInTimeStep; // time cost for the simulation

        if (-(pow(rocketVelocity, 2)) / (2 * (distancetoDecelerate - landing_range - distanceDecelerated)) < acceleration) { // 0 meter per second is the goal velocity
            currentEFMI += 1;
        }
        else if (currentEFMI >= 1) {
            currentEFMI -= 1;
        }
        if (acceleration < -10) {
            currentEFMI -= 2;
        }

        if (rocketVelocity < landing_range && rocketVelocity > 0) {
            rocketVelocity = 0;
            currentEFMI = 0; // Stop engine if fuel runs out
            thrust = 0;
            landing_phrase = false;
            // update rocket sprite
            if (!rocketTexture.loadFromFile("assets/texture/rocket_noflame.png")) { // Replace with your rocket image path
                std::cerr << "Failed to load rocket image" << std::endl;
                return -1;
            }

            // Set the texture for the rocket sprite
            rocket.setTexture(&rocketTexture);
        }

        if (count % 1000 == 0 && write) {
            writeDataToCSV(count / 1000, x, rocketVelocity);
        }

        if (visual) {
            if (window.isOpen()) {
                window.clear();

                rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * ((x - totalDistanceCovered) / (d - totalDistanceCovered))); // Initial position of the rocket
                text_rocket_velocity.setString("Rocket Speed: " + std::to_string(rocketVelocity) + " m/s");
                text_rocket_acceleration.setString("Rocket Acceleration: " + std::to_string(acceleration) + " m/s^2");
                text_rocket_fuel.setString("Rocket Fuel: " + std::to_string(currentFuelMass) + " kg");
                text_rocket_time.setString("Rocket Time: " + std::to_string(timeExpended_4 * timeStep) + " seconds");
                text_rocket_displacement.setString("Rocket Distance: " + std::to_string(x / 1e3) + " km");
                text_rocket_cfmi.setString("Current Fuel Mass Input: " + std::to_string(currentEFMI));
                text_simulation_phase.setString("Landing!");

                window.draw(backgroundSprite); // Draw the background
                window.draw(moonSprite); // Draw the moon overlay
                window.draw(text_simulation_phase); // Draw the text_simulation_phase text
                window.draw(text_rocket_velocity); // Draw the rocket speed text
                window.draw(text_rocket_acceleration); // Draw the rocket acceleration text
                window.draw(text_rocket_fuel); // Draw the rocket fuel text
                window.draw(text_rocket_time); // Draw the rocket time text
                window.draw(text_rocket_displacement); // Draw the rocket distance text
                window.draw(text_rocket_cfmi);
                window.draw(rocket);
                // sf::sleep(sf::milliseconds(1)); // Sleep for 1 millisecond to control the frame rate

                window.display();
            }
        }
        count++;
    }
    if (rocketVelocity < 0) {
        time_cost_4 = 1e3;
        fuel_cost_4 = 1e3;
    }
    else {
        // normalize timeExpended to seconds
        time_cost_4 /= 1e3; // normalize time cost to seconds/km
        time_cost_4 += rocketVelocity; // time cost for the simulation
        fuel_cost_4 += (rocketMassPostDecelerating - currentFuelMass) / work; // fuel cost for the simulation
    }
    double cost_landing = time_cost_4 * timeEfficiencyConstant + fuel_cost_4 * fuelEfficiencyConstant + time_cost_4 * fuel_cost_4; // cost of the simulation

    timeExpended_4 *= timeStep; // normalize timeExpended to seconds
    std::cout << "------------------------------------------------Stage 4 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << timeExpended_4 << " seconds\n";
    std::cout << "Cruise velocity: " << rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (x) / 1e3 << " km\n";
    std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";
    std::cout << "Distance Left: " << (d - x) << "m" << std::endl;
    std::cout << "time cost: " << time_cost_4 << " s/km\n";
    std::cout << "Fuel Cost: " << fuel_cost_4 << " Kg/j\n";
    std::cout << "Cost of the simulation: " << cost_landing << std::endl;
    std::cout << "------------------------------------------------stats----------------------------------------------\n" << std::endl;
    std::cout << "Total Time: " << timeExpended_1 + timeExpended_2 + timeExpended_3 + timeExpended_4 << " seconds\n";
    std::cout << "Total Distance: " << (x) / 1e3 << " km\n";

    std::cout << "total time cost: " << time_cost_1 + time_cost_2 + time_cost_3 + time_cost_4 << " s/km\n";
    std::cout << "total fuel cost: " << fuel_cost_1 + fuel_cost_3 + fuel_cost_4 << " Kg/j\n";
    std::cout << "total cost of the simulation: " << cost_liftoff + cost_cruise + cost_decelerate + cost_landing << std::endl;

    while (visual && window.isOpen()) {
        sf::Event event; // event catcher to close window

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
    }

    return 0;
}
