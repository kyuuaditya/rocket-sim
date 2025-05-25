#include <iostream>
#include <cmath>
#include <sfml/Graphics.hpp>
#include "flight_data.h"
#include "constants.h" 
#include "rocket.h"
#include "print_data.h"
#include "graphics.h"

bool visual = true; // Set to true for visual simulation,
bool write = false; // set to true to write to CSV
double target_velocity = 16000;
double braking_distance_s3 = 2000 * 1e3; // 2000 km

int main() {
    VisualData VisualData;
    Constants Constants;
    Rocket Rocket;
    printData printData;
    Graphics Graphics;

    // time step for simulation
    long double timeStep = 0.001; // in seconds

    // create the window for the simulation
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "simulation", sf::Style::None);

    Graphics.load(0);
    Graphics.initialize(window); // Initialize graphics with the window

    VisualData.load();
    VisualData.initialize();

    int count = 0; // counter for simulation steps
    double work = 0; // cost of the simulation
    double timeEfficiencyConstant = 1; // time efficiency of the simulation
    double fuelEfficiencyConstant = 1; // fuel efficiency of the simulation
    double time_cost_1 = 0;

    bool is_game_paused = false;

    // -------------------------------------------------------------- Stage 1 ----------------------------------------------------------
    while (Rocket.rocketVelocity < target_velocity) { // Run for 5 minutes
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
            if (Rocket.currentFuelMass > 0) {
                // calculating Rocket.thrust
                Rocket.thrust = sqrt(2 * Rocket.enginePower * Rocket.currentEFMI);

                Rocket.exhaustVelocity = sqrt((2 * Rocket.enginePower) / Rocket.currentEFMI); // calculate exhaust velocity

                // Burn fuel
                Rocket.currentFuelMass -= Rocket.currentEFMI * timeStep;
                if (Rocket.currentFuelMass < 0) {
                    break;
                    Rocket.currentFuelMass += Rocket.currentEFMI * timeStep;
                    Rocket.currentEFMI = 0; // Stop engine if fuel runs out
                    Rocket.thrust = 0;
                }

                // Update rocket mass
                Rocket.currentRocketMass = Rocket.currentFuelMass + Rocket.rocketDryMass;
            }

            // Correct net force calculation
            Rocket.netForce = Rocket.thrust
                - ((Constants.GravitationalConstant * Constants.massEarth * Rocket.currentRocketMass) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2))
                + ((Constants.GravitationalConstant * Constants.massMoon * Rocket.currentRocketMass) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2));

            // Rocket.acceleration update
            Rocket.acceleration = Rocket.netForce / Rocket.currentRocketMass;
            // if Rocket.acceleration is too high reduce fuel burn rate
            if (Rocket.acceleration >= 50 && Rocket.currentEFMI > 1) {
                Rocket.currentEFMI -= 1;
            }

            // update rocket velocity and position
            long double distanceInTimeStep = Rocket.rocketVelocity * timeStep + 0.5 * Rocket.acceleration * pow(timeStep, 2);
            work += distanceInTimeStep * Rocket.netForce;
            Rocket.rocketVelocity += Rocket.acceleration * timeStep;
            Rocket.netDisplacement += distanceInTimeStep;
            Rocket.timeExpended_1 += 1;
            time_cost_1 += timeStep / distanceInTimeStep;

            if (count % 1000 == 0 && (write)) {
                printData.writeDataToCSV(count / 1000, Rocket.netDisplacement, Rocket.rocketVelocity, write);
            }
        }

        if (visual) {
            if (count % 20 == 0 && window.isOpen()) { // Update every 20 instances for smoother animation
                window.clear();

                Graphics.update(window, Rocket.netDisplacement, 0, Constants.GapEarthMoon); // Update the rocket's position based on its current state
                VisualData.update(sf::seconds(Rocket.timeExpended_1 * timeStep), Rocket.rocketVelocity, Rocket.acceleration, Rocket.currentFuelMass, Rocket.netDisplacement, Rocket.currentEFMI, Rocket.exhaustVelocity, ((Constants.GravitationalConstant * Constants.massEarth) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2)), ((Constants.GravitationalConstant * Constants.massMoon) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2)), "Liftoff!");

                Graphics.draw(window, 0); // Draw the graphics
                VisualData.draw(window); // Draw the visual data

                window.display();
            }
        }
        count++;

    }
    // normalize timeExpended to seconds
    Rocket.timeExpended_1 *= timeStep;
    time_cost_1 /= 1e3; // normalize time cost to seconds/km
    time_cost_1 += 1e3 / sqrt(Rocket.rocketVelocity);
    double fuel_cost_1 = (Rocket.initialFuelMass - Rocket.currentFuelMass) / work;
    double cost_liftoff = time_cost_1 * timeEfficiencyConstant + fuel_cost_1 * fuelEfficiencyConstant + time_cost_1 * fuel_cost_1; // cost of the simulation

    std::cout << "------------------------------------------------Stage 1 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << Rocket.timeExpended_1 << " seconds\n";
    std::cout << "Final velocity: " << Rocket.rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << Rocket.netDisplacement / 1e3 << " km\n";
    std::cout << "Distance Left: " << (Constants.GapEarthMoon - Rocket.netDisplacement) / 1e3 << "km" << std::endl;
    std::cout << "Current Rocket Mass: " << Rocket.currentRocketMass << " kg\n";
    std::cout << "Fuel consumed: " << Rocket.initialFuelMass - Rocket.currentFuelMass << " kg " << "Fuel remaining: " << Rocket.currentFuelMass << " kg\n";
    std::cout << "Fuel Cost: " << fuel_cost_1 << " Kg/j\n";
    std::cout << "Time Cost: " << time_cost_1 << " s/km\n";
    std::cout << "Cost of the simulation: " << cost_liftoff << std::endl;

    // -------------------------------------------------------------- State 2 ----------------------------------------------------------

    Graphics.load(1);

    // initialize rocket statistics
    double cruiseVelocity = Rocket.rocketVelocity;
    double distanceToCruise = Constants.GapEarthMoon - Rocket.netDisplacement; // Distance to cruise
    double distanceCruised = 0;
    Rocket.currentEFMI = 0; // No fuel burn rate during cruise

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
        Rocket.netForce =
            ((Constants.GravitationalConstant * Constants.massMoon * Rocket.currentRocketMass) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2))
            - ((Constants.GravitationalConstant * Constants.massEarth * Rocket.currentRocketMass) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2));

        Rocket.acceleration = Rocket.netForce / Rocket.currentRocketMass;

        // update rocket velocity and position
        long double distanceInTimeStep = cruiseVelocity * timeStep + 0.5 * Rocket.acceleration * pow(timeStep, 2);
        cruiseVelocity += Rocket.acceleration * timeStep;
        Rocket.netDisplacement += distanceInTimeStep;
        distanceCruised += distanceInTimeStep;
        timeExpended_2 += 1;

        time_cost_2 += timeStep / distanceInTimeStep; // time cost for the simulation

        if (write) {
            printData.writeDataToCSV(count, Rocket.netDisplacement, cruiseVelocity, write);
        }

        if (visual) {
            // Update rocket position for visualization
            if (window.isOpen()) {
                window.clear();

                Graphics.update(window, Rocket.netDisplacement, 1, Constants.GapEarthMoon); // Update the rocket's position based on its current state
                VisualData.update(sf::seconds(timeExpended_2 * timeStep), cruiseVelocity, Rocket.acceleration, Rocket.currentFuelMass, Rocket.netDisplacement, Rocket.currentEFMI, Rocket.exhaustVelocity, ((Constants.GravitationalConstant * Constants.massEarth) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2)), ((Constants.GravitationalConstant * Constants.massMoon) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2)), "Cruising!");

                Graphics.draw(window, 1); // Draw the graphics
                VisualData.draw(window); // Draw the visual data

                window.display();
            }
        }

        count++;
    }

    if (cruiseVelocity < 0) {
        std::cout << "Rocket has stopped moving" << std::endl;
        Rocket.currentEFMI = 0; // Stop engine if fuel runs out
        Rocket.thrust = 0;
        rocketStatus = false; // Stop the rocket
    }


    std::cout << "------------------------------------------------Stage 2 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << timeExpended_2 << " seconds\n";
    std::cout << "Cruise velocity: " << cruiseVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceCruised / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (Rocket.netDisplacement) / 1e3 << " km\n";
    std::cout << "Distance Left: " << (Constants.GapEarthMoon - Rocket.netDisplacement) / 1e3 << "km" << std::endl;
    std::cout << "time cost: " << time_cost_2 << " s/km\n";
    double cost_cruise = time_cost_2;

    // -------------------------------------------------------------- State 3 ----------------------------------------------------------

    Graphics.load(2);

    // initialize rocket statistics
    Rocket.rocketVelocity = cruiseVelocity; // Reset velocity to cruise velocity
    double distancetoDecelerate = Constants.GapEarthMoon - Rocket.netDisplacement; // Distance to decelerate
    double totalDistanceCovered = Rocket.netDisplacement;
    double distanceDecelerated = 0;
    Rocket.thrust = 0;
    Rocket.netForce = 0;
    Rocket.currentEFMI = 0;
    double rocketMassPostCruise = Rocket.currentFuelMass;

    double timeExpended_3 = 0;

    count = 0;
    work = 0;
    double time_cost_3 = 0; // time cost for the simulation
    double fuel_cost_3 = 0; // fuel cost for the simulation

    timeStep = 0.001; // in seconds

    while (distancetoDecelerate - 1 * 1e3 > distanceDecelerated && Rocket.rocketVelocity > 0) {
        sf::Event event; // event catcher to close window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        if (Rocket.currentFuelMass > 0) {
            // calculating Rocket.thrust
            Rocket.thrust = -sqrt(2 * Rocket.enginePower * Rocket.currentEFMI); // Negative Rocket.thrust for braking

            Rocket.exhaustVelocity = sqrt((2 * Rocket.enginePower) / Rocket.currentEFMI); // calculate exhaust velocity

            // Burn fuel
            Rocket.currentFuelMass -= Rocket.currentEFMI * timeStep;
            if (Rocket.currentFuelMass < 0) {
                break;
                Rocket.currentFuelMass += Rocket.currentEFMI * timeStep;
                Rocket.currentEFMI = 0; // Stop engine if fuel runs out
                Rocket.thrust = 0;
            }

            // Update rocket mass
            Rocket.currentRocketMass = Rocket.currentFuelMass + Rocket.rocketDryMass;
        }

        // Net force calculation
        Rocket.netForce = Rocket.thrust
            - ((Constants.GravitationalConstant * Constants.massEarth * Rocket.currentRocketMass) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2))
            + ((Constants.GravitationalConstant * Constants.massMoon * Rocket.currentRocketMass) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2));

        Rocket.acceleration = Rocket.netForce / Rocket.currentRocketMass;

        // Update rocket velocity and position
        long double distanceInTimeStep = Rocket.rocketVelocity * timeStep + 0.5 * Rocket.acceleration * pow(timeStep, 2);
        Rocket.rocketVelocity += Rocket.acceleration * timeStep;
        Rocket.netDisplacement += distanceInTimeStep;
        distanceDecelerated += distanceInTimeStep;
        timeExpended_3 += 1;
        work += -distanceInTimeStep * Rocket.netForce;
        time_cost_3 += timeStep / distanceInTimeStep; // time cost for the simulation

        // change efmi as per goal final velocity ie. 100 m/s
        if ((-(pow(Rocket.rocketVelocity, 2) - pow(100, 2)) / (2 * (distancetoDecelerate - 1 * 1e3 - distanceDecelerated)) < Rocket.acceleration) && Rocket.acceleration > -50) {
            Rocket.currentEFMI += 1;
        }
        else {
            if (Rocket.currentEFMI >= 1) Rocket.currentEFMI -= 1;
        }
        if (Rocket.acceleration < -50 && Rocket.currentEFMI >= 1) Rocket.currentEFMI -= 1; // safety check for 5g


        if (count % 1000 == 0 && write) {
            printData.writeDataToCSV(count / 1000, Rocket.netDisplacement, Rocket.rocketVelocity, write);
        }

        if (count % 10 == 0) {
            if (visual) {
                window.clear();

                Graphics.update(window, Rocket.netDisplacement, 2, Constants.GapEarthMoon, totalDistanceCovered); // Update the rocket's position based on its current state
                VisualData.update(sf::seconds(Rocket.timeExpended_1 + timeExpended_2 + timeExpended_3 * timeStep), Rocket.rocketVelocity, Rocket.acceleration, Rocket.currentFuelMass, Rocket.netDisplacement, Rocket.currentEFMI, Rocket.exhaustVelocity, ((Constants.GravitationalConstant * Constants.massEarth) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2)), ((Constants.GravitationalConstant * Constants.massMoon) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2)), "Decelerating!");

                Graphics.draw(window, 2); // Draw the graphics
                VisualData.draw(window); // Draw the visual data

                window.display();
            }
        }

        count++;
    }
    if (Rocket.rocketVelocity < 0) {
        time_cost_3 = 1e3;
        fuel_cost_3 = 1e3;
    }
    else {
        time_cost_3 /= 1e3; // normalize time cost to seconds/km
        time_cost_3 += Rocket.rocketVelocity / 100; // time cost for the simulation
        fuel_cost_3 += (rocketMassPostCruise - Rocket.currentFuelMass) / work; // fuel cost for the simulation
    }
    timeExpended_3 *= timeStep;
    double cost_decelerate = time_cost_3 * timeEfficiencyConstant + fuel_cost_3 * fuelEfficiencyConstant + time_cost_3 * fuel_cost_3; // cost of the simulation


    std::cout << "------------------------------------------------Stage 3 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << timeExpended_3 << " seconds\n";
    std::cout << "Cruise velocity: " << Rocket.rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (Rocket.netDisplacement) / 1e3 << " km\n";
    std::cout << "Fuel consumed: " << Rocket.initialFuelMass - Rocket.currentFuelMass << " kg " << "Fuel remaining: " << Rocket.currentFuelMass << " kg\n";
    std::cout << "Distance Left: " << (Constants.GapEarthMoon - Rocket.netDisplacement) << "m\n";
    std::cout << "time cost: " << time_cost_3 << " s/km\n";
    std::cout << "Fuel Cost: " << fuel_cost_3 << " Kg/j\n";
    std::cout << "Cost of the simulation: " << cost_decelerate << std::endl;
    // -------------------------------------------------------------- State 4 ----------------------------------------------------------
    Rocket.enginePower = 1e10; // watts

    // initialize rocket statistics
    distancetoDecelerate = Constants.GapEarthMoon - Rocket.netDisplacement;
    totalDistanceCovered = Rocket.netDisplacement;
    Rocket.currentEFMI = 0;
    Rocket.thrust = 0;
    Rocket.netForce = 0;
    distanceDecelerated = 0;
    double landing_range = 1; // 1 meter
    bool landing_phrase = true;
    double time_cost_4 = 0; // time cost for the simulation
    double fuel_cost_4 = 0; // fuel cost for the simulation
    double rocketMassPostDecelerating = Rocket.currentFuelMass;

    count = 0;
    double timeExpended_4 = 0;

    timeStep = 0.001; // in seconds

    while (distancetoDecelerate - landing_range > distanceDecelerated && Rocket.netDisplacement < Constants.GapEarthMoon && landing_phrase && rocketStatus) {
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

        if (Rocket.currentFuelMass > 0) {
            Rocket.thrust = -sqrt(2 * Rocket.enginePower * Rocket.currentEFMI); // Negative Rocket.thrust for braking

            Rocket.exhaustVelocity = sqrt((2 * Rocket.enginePower) / Rocket.currentEFMI); // calculate exhaust velocity

            // Burn fuel
            Rocket.currentFuelMass -= Rocket.currentEFMI * timeStep;
            if (Rocket.currentFuelMass < 0) {
                std::cout << "Ran out of fuel when landing" << std::endl;
                Rocket.currentFuelMass += Rocket.currentEFMI * timeStep;
                Rocket.currentEFMI = 0; // Stop engine if fuel runs out
                Rocket.thrust = 0;
                break;
            }

            // Update rocket mass
            Rocket.currentRocketMass = Rocket.currentFuelMass + Rocket.rocketDryMass;
        }

        // Correct net force calculation
        Rocket.netForce = Rocket.thrust
            - ((Constants.GravitationalConstant * Constants.massEarth * Rocket.currentRocketMass) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2))
            + ((Constants.GravitationalConstant * Constants.massMoon * Rocket.currentRocketMass) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2));

        // Rocket.acceleration update
        Rocket.acceleration = Rocket.netForce / Rocket.currentRocketMass;

        // update rocket velocity and position
        long double distanceInTimeStep = Rocket.rocketVelocity * timeStep + 0.5 * Rocket.acceleration * pow(timeStep, 2);
        Rocket.rocketVelocity += Rocket.acceleration * timeStep;
        Rocket.netDisplacement += distanceInTimeStep;

        distanceDecelerated += distanceInTimeStep;
        timeExpended_4 += 1;
        work += -distanceInTimeStep * Rocket.netForce;
        time_cost_4 += timeStep / distanceInTimeStep; // time cost for the simulation

        if (-(pow(Rocket.rocketVelocity, 2)) / (2 * (distancetoDecelerate - landing_range - distanceDecelerated)) < Rocket.acceleration) { // 0 meter per second is the goal velocity
            Rocket.currentEFMI += 1;
        }
        else if (Rocket.currentEFMI >= 1) {
            Rocket.currentEFMI -= 1;
        }
        if (Rocket.acceleration < -10) {
            Rocket.currentEFMI -= 2;
        }

        if (Rocket.rocketVelocity < landing_range && Rocket.rocketVelocity > 0) {
            Rocket.rocketVelocity = 0;
            Rocket.currentEFMI = 0; // Stop engine if fuel runs out
            Rocket.thrust = 0;
            landing_phrase = false;
            Graphics.load(3); // Load the landing graphics
        }

        if (count % 1000 == 0 && write) {
            printData.writeDataToCSV(count / 1000, Rocket.netDisplacement, Rocket.rocketVelocity, write);
        }

        if (visual) {
            if (window.isOpen()) {
                window.clear();

                Graphics.update(window, Rocket.netDisplacement, 3, Constants.GapEarthMoon, totalDistanceCovered); // Update the rocket's position based on its current state
                VisualData.update(sf::seconds(Rocket.timeExpended_1 + timeExpended_2 + timeExpended_3 + timeExpended_4 * timeStep), Rocket.rocketVelocity, Rocket.acceleration, Rocket.currentFuelMass, Rocket.netDisplacement, Rocket.currentEFMI, Rocket.exhaustVelocity, ((Constants.GravitationalConstant * Constants.massEarth) / pow((Constants.radiusEarth + Rocket.netDisplacement), 2)), ((Constants.GravitationalConstant * Constants.massMoon) / pow((Constants.radiusMoon + Constants.GapEarthMoon - Rocket.netDisplacement), 2)), "Landing!");

                Graphics.draw(window, 3); // Draw the graphics
                VisualData.draw(window); // Draw the visual data

                window.display();
            }
        }
        count++;
    }
    if (Rocket.rocketVelocity < 0) {
        time_cost_4 = 1e3;
        fuel_cost_4 = 1e3;
    }
    else {
        // normalize timeExpended to seconds
        time_cost_4 /= 1e3; // normalize time cost to seconds/km
        time_cost_4 += Rocket.rocketVelocity; // time cost for the simulation
        fuel_cost_4 += (rocketMassPostDecelerating - Rocket.currentFuelMass) / work; // fuel cost for the simulation
    }
    double cost_landing = time_cost_4 * timeEfficiencyConstant + fuel_cost_4 * fuelEfficiencyConstant + time_cost_4 * fuel_cost_4; // cost of the simulation

    timeExpended_4 *= timeStep; // normalize timeExpended to seconds
    std::cout << "------------------------------------------------Stage 4 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << timeExpended_4 << " seconds\n";
    std::cout << "Cruise velocity: " << Rocket.rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (Rocket.netDisplacement) / 1e3 << " km\n";
    std::cout << "Fuel consumed: " << Rocket.initialFuelMass - Rocket.currentFuelMass << " kg " << "Fuel remaining: " << Rocket.currentFuelMass << " kg\n";
    std::cout << "Distance Left: " << (Constants.GapEarthMoon - Rocket.netDisplacement) << "m" << std::endl;
    std::cout << "time cost: " << time_cost_4 << " s/km\n";
    std::cout << "Fuel Cost: " << fuel_cost_4 << " Kg/j\n";
    std::cout << "Cost of the simulation: " << cost_landing << std::endl;
    std::cout << "------------------------------------------------stats----------------------------------------------\n" << std::endl;
    std::cout << "Total Time: " << Rocket.timeExpended_1 + timeExpended_2 + timeExpended_3 + timeExpended_4 << " seconds\n";
    std::cout << "Total Distance: " << (Rocket.netDisplacement) / 1e3 << " km\n";

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
