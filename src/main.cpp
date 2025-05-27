#include <iostream>
#include <cmath>
#include <sfml/Graphics.hpp>
#include "flight_data.h"
#include "constants.h" 
#include "rocket.h"
#include "print_data.h"
#include "graphics.h"

bool visual = true; // Set to true for visual simulation,
bool write = true; // set to true to write to CSV
double target_velocity = 16000;
double braking_distance_s3 = 2000 * 1e3; // 2000 km

int main() {
    // initialize the objects
    VisualData VisualData;
    Constants Constants;
    Rocket Rocket;
    printData printData;
    Graphics Graphics;

    // create the window for the simulation
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "simulation", sf::Style::None);

    Graphics.load(0);
    Graphics.initialize(window); // Initialize graphics with the window

    VisualData.load();
    VisualData.initialize();

    int count = 0; // counter for simulation steps
    long double timeStep = 0.001; // updates the simulation every 1 ms. 
    bool is_game_paused = false; // determines game pause state

    // -------------------------------------------------------------- Stage 1 ----------------------------------------------------------
    while (Rocket.rocketVelocity < target_velocity) { // Burns till the rocket reaches the target velocity
        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                is_game_paused = (is_game_paused) ? false : true; // toggle is_game_paused state
            }
        }

        if (!is_game_paused) {
            // check if fuel is available
            Rocket.rocketBurn(timeStep); // Burn fuel and update thrust, exhaust velocity, and current rocket mass

            Rocket.rocketMove(timeStep); // Move the rocket based on the current state

            if (Rocket.acceleration >= 50 && Rocket.currentEFMI > 1) {
                Rocket.currentEFMI -= 1;
            }
            // if (count % 1000 == 0 && (write)) { // write data every 1 second 
            //     printData.writeDataToCSV(count / 1000, Rocket.netDisplacement, Rocket.rocketVelocity, write);
            // }
        }

        if (visual) {
            if (count % 20 == 0 && window.isOpen()) { // Update every 20 instances for smoother animation
                window.clear();

                Graphics.update(window, Rocket.netDisplacement, 0, Constants.GapEarthMoon); // Update the rocket's position based on its current state
                VisualData.update(sf::seconds(Rocket.timeExpended_m * timeStep), Rocket.rocketVelocity, Rocket.acceleration, Rocket.currentFuelMass, Rocket.netDisplacement, Rocket.currentEFMI, Rocket.exhaustVelocity, ((Constants.GravitationalConstant * Constants.massEarth) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2)), ((Constants.GravitationalConstant * Constants.massMoon) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2)), "Liftoff!");

                Graphics.draw(window, 0); // Draw the graphics
                VisualData.draw(window); // Draw the visual data

                window.display();
            }
        }
        count++;

    }
    // normalize timeExpended to seconds
    Rocket.timeExpended_m *= timeStep;
    Rocket.timeExpended_s += Rocket.timeExpended_m; // Convert timeExpended from milliseconds to seconds

    std::cout << "------------------------------------------------Stage 1 Complete----------------------------------------------" << std::endl;
    printData.print_to_terminal(0, Rocket.timeExpended_s, Rocket.rocketVelocity, Rocket.netDisplacement, Constants.GapEarthMoon, Rocket.currentRocketMass, Rocket.initialFuelMass, Rocket.currentFuelMass);

    // is_game_paused = true; // Pause the game after stage 1
    // -------------------------------------------------------------- State 2 ----------------------------------------------------------

    Graphics.load(1);

    // initialize rocket statistics
    double distanceToCruise = Constants.GapEarthMoon - Rocket.netDisplacement; // Distance to cruise
    double distanceCruised = 0;
    Rocket.currentEFMI = 0; // No fuel burn rate during cruise

    count = 0;
    bool rocketStatus = true; // Rocket status

    Rocket.thrust = 0; // No thrust during cruise
    Rocket.exhaustVelocity = 0; // No exhaust velocity during cruise
    Rocket.currentEFMI = 0; // No fuel burn rate during cruise
    Rocket.acceleration = 0; // No acceleration during cruise
    Rocket.timeExpended_m = 0; // Reset timeExpended for this stage

    timeStep = 1; // in seconds
    while (Rocket.distanceCruised + braking_distance_s3 < distanceToCruise) { // 30 hours for reference
        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                is_game_paused = (is_game_paused) ? false : true; // toggle is_game_paused state
            }
        }

        if (!is_game_paused) {
            Rocket.rocketMove(timeStep, 1); // Move the rocket based on the current state

            // if (write) {
            //     printData.writeDataToCSV(count, Rocket.netDisplacement, cruiseVelocity, write);
            // }
        }

        if (visual) {
            // Update rocket position for visualization
            if (window.isOpen()) {
                window.clear();

                Graphics.update(window, Rocket.netDisplacement, 1, Constants.GapEarthMoon); // Update the rocket's position based on its current state
                VisualData.update(sf::seconds(Rocket.timeExpended_m * timeStep), Rocket.rocketVelocity, Rocket.acceleration, Rocket.currentFuelMass, Rocket.netDisplacement, Rocket.currentEFMI, Rocket.exhaustVelocity, ((Constants.GravitationalConstant * Constants.massEarth) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2)), ((Constants.GravitationalConstant * Constants.massMoon) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2)), "Cruising!");

                Graphics.draw(window, 1); // Draw the graphics
                VisualData.draw(window); // Draw the visual data

                window.display();
            }
        }

        count++;
    }

    if (Rocket.rocketVelocity < 0) {
        std::cout << "Rocket has stopped moving up" << std::endl;
        Rocket.currentEFMI = 0; // Stop engine if fuel runs out
        Rocket.thrust = 0;
        rocketStatus = false; // Stop the rocket
    }

    Rocket.timeExpended_m *= timeStep; // Update timeExpended in milliseconds
    Rocket.timeExpended_s += Rocket.timeExpended_m; // Convert timeExpended from milliseconds to seconds

    std::cout << "------------------------------------------------Stage 2 Complete----------------------------------------------" << std::endl;
    printData.print_to_terminal(1, Rocket.timeExpended_s, Rocket.rocketVelocity, Rocket.netDisplacement, Constants.GapEarthMoon, Rocket.currentRocketMass, Rocket.initialFuelMass, Rocket.currentFuelMass, distanceCruised);

    // -------------------------------------------------------------- State 3 ----------------------------------------------------------

    Graphics.load(2);

    // initialize rocket statistics
    double distancetoDecelerate = Constants.GapEarthMoon - Rocket.netDisplacement; // Distance to decelerate
    double totalDistanceCovered = Rocket.netDisplacement;
    double distanceDecelerated = 0;
    Rocket.thrust = 0;
    Rocket.netForce = 0;
    Rocket.currentEFMI = 0;
    double rocketMassPostCruise = Rocket.currentFuelMass;

    count = 0;

    timeStep = 0.001; // in seconds

    while (distancetoDecelerate - 1 * 1e3 > Rocket.distanceDecelerated && Rocket.rocketVelocity > 0) {
        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                is_game_paused = (is_game_paused) ? false : true; // toggle is_game_paused state
            }
        }

        if (!is_game_paused) {

            Rocket.rocketBurn(timeStep); // Burn fuel and update thrust, exhaust velocity, and current rocket mass

            Rocket.rocketMove(timeStep, 2); // Move the rocket based on the current state

            // change efmi as per goal final velocity ie. 100 m/s
            if ((-(pow(Rocket.rocketVelocity, 2) - pow(100, 2)) / (2 * (distancetoDecelerate - 1 * 1e3 - Rocket.distanceDecelerated)) < Rocket.acceleration) && Rocket.acceleration > -50) {
                Rocket.currentEFMI += 1;
            }
            else {
                if (Rocket.currentEFMI >= 1) Rocket.currentEFMI -= 1;
            }

            if (count % 1000 == 0 && write) {
                printData.writeDataToCSV(count / 1000, Rocket.netDisplacement, Rocket.rocketVelocity, write);
            }
        }

        if (count % 10 == 0) {
            if (visual) {
                window.clear();

                Graphics.update(window, Rocket.netDisplacement, 2, Constants.GapEarthMoon, totalDistanceCovered); // Update the rocket's position based on its current state
                VisualData.update(sf::seconds(Rocket.timeExpended_m * timeStep), Rocket.rocketVelocity, Rocket.acceleration, Rocket.currentFuelMass, Rocket.netDisplacement, Rocket.currentEFMI, Rocket.exhaustVelocity, ((Constants.GravitationalConstant * Constants.massEarth) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2)), ((Constants.GravitationalConstant * Constants.massMoon) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2)), "Decelerating!");

                Graphics.draw(window, 2); // Draw the graphics
                VisualData.draw(window); // Draw the visual data

                window.display();
            }
        }

        count++;
    }
    Rocket.timeExpended_m *= timeStep; // Update timeExpended in milliseconds
    Rocket.timeExpended_s += Rocket.timeExpended_m; // Convert timeExpended from milliseconds to seconds

    std::cout << "------------------------------------------------Stage 3 Complete----------------------------------------------" << std::endl;
    printData.print_to_terminal(2, Rocket.timeExpended_s, Rocket.rocketVelocity, Rocket.netDisplacement, Constants.GapEarthMoon, Rocket.currentRocketMass, Rocket.initialFuelMass, Rocket.currentFuelMass, Rocket.distanceDecelerated);

    // -------------------------------------------------------------- State 4 ----------------------------------------------------------
    Rocket.enginePower = 1e10; // new engine for landing

    // initialize rocket statistics
    distancetoDecelerate = Constants.GapEarthMoon - Rocket.netDisplacement;
    totalDistanceCovered = Rocket.netDisplacement;
    Rocket.currentEFMI = 0;
    Rocket.thrust = 0;
    Rocket.netForce = 0;
    distanceDecelerated = 0;
    double landing_range = 1; // 1 meter
    bool landing_phrase = true;
    double rocketMassPostDecelerating = Rocket.currentFuelMass;
    Rocket.timeExpended_m = 0; // Reset timeExpended for this stage
    timeStep = 0.001; // in seconds
    Rocket.distanceDecelerated = 0;

    count = 0;

    timeStep = 0.001; // in seconds
    std::cout << (distancetoDecelerate - landing_range > Rocket.distanceDecelerated) << (Rocket.netDisplacement < Constants.GapEarthMoon) << landing_phrase << rocketStatus << std::endl;
    while (distancetoDecelerate - landing_range > Rocket.distanceDecelerated && Rocket.netDisplacement < Constants.GapEarthMoon && landing_phrase && rocketStatus) {
        // if (count * timeStep > 100) { // 100 seconds allowed for landing
        //     break;
        // }

        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                is_game_paused = (is_game_paused) ? false : true; // toggle is_game_paused state
            }
        }

        if (!is_game_paused) {

            Rocket.rocketBurn(timeStep); // Burn fuel and update thrust, exhaust velocity, and current rocket mass

            Rocket.rocketMove(timeStep, 3); // Move the rocket based on the current state

            if (-(pow(Rocket.rocketVelocity, 2)) / (2 * (distancetoDecelerate - landing_range - Rocket.distanceDecelerated)) < Rocket.acceleration) { // 0 meter per second is the goal velocity
                Rocket.currentEFMI += 1;
            }
            else if (Rocket.currentEFMI >= 1) Rocket.currentEFMI -= 1;

            if (Rocket.acceleration < -10) Rocket.currentEFMI -= 2;

            if (Rocket.rocketVelocity < landing_range && Rocket.rocketVelocity > 0) {
                Rocket.rocketVelocity = 0;
                Rocket.currentEFMI = 0; // Stop engine if fuel runs out
                Rocket.thrust = 0;
                landing_phrase = false;
                Graphics.load(3); // Load the landing graphics
            }

            // if (count % 1000 == 0 && write) {
            //     printData.writeDataToCSV(count / 1000, Rocket.netDisplacement, Rocket.rocketVelocity, write);
            // }
        }

        if (visual) {
            if (window.isOpen()) {
                window.clear();

                Graphics.update(window, Rocket.netDisplacement, 3, Constants.GapEarthMoon, totalDistanceCovered); // Update the rocket's position based on its current state
                VisualData.update(sf::seconds(Rocket.timeExpended_m * timeStep), Rocket.rocketVelocity, Rocket.acceleration, Rocket.currentFuelMass, Rocket.netDisplacement, Rocket.currentEFMI, Rocket.exhaustVelocity, ((Constants.GravitationalConstant * Constants.massEarth) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2)), ((Constants.GravitationalConstant * Constants.massMoon) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2)), "Landing!");

                Graphics.draw(window, 3); // Draw the graphics
                VisualData.draw(window); // Draw the visual data

                window.display();
            }
        }
        count++;
    }
    Rocket.timeExpended_m *= timeStep; // Update timeExpended in milliseconds
    Rocket.timeExpended_s += Rocket.timeExpended_m; // Convert timeExpended from milliseconds to seconds

    std::cout << "------------------------------------------------Stage 4 Complete----------------------------------------------" << std::endl;
    printData.print_to_terminal(3, Rocket.timeExpended_s, Rocket.rocketVelocity, Rocket.netDisplacement, Constants.GapEarthMoon, Rocket.currentRocketMass, Rocket.initialFuelMass, Rocket.currentFuelMass, Rocket.distanceDecelerated);

    std::cout << "------------------------------------------------stats----------------------------------------------" << std::endl;
    std::cout << "Total Time: " << Rocket.timeExpended_s << " seconds\n";
    std::cout << "Total Distance: " << (Rocket.netDisplacement) / 1e3 << " km\n";

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
