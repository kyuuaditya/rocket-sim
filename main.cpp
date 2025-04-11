#include <iostream>
#include <cmath>
#include <sfml/Graphics.hpp>

// std::setprecision(10); // Set precision for output

int main() {
    bool visual = false; // Set to true for visual simulation, false for console output

    const long double massEarth = 5.972e24; // kg
    const long double radiusEarth = 6.378e6; // meters

    const long double massSun = 1.989e30; // kg
    const long double radiusSun = 6.9634e8; // meters

    const long double massMoon = 7.347e22; // kg
    const long double radiusMoon = 1.7374e6; // meters

    const long double d = 3.844006e8 - radiusEarth - radiusMoon; // Distance between Earth & Moon minus their radii
    const long double G = 6.674e-11; // gravitational constant
    const long double S = 1.496e11; // distance from earth to sun (centres)

    const long double enginePower = 220.0e9; // watts

    long double rocketTotalMass = 1.0e6; // kg
    long double rocketDryMass = 1.35e5; // kg
    long double initialFuelMass = rocketTotalMass - rocketDryMass;
    long double currentFuelMass = initialFuelMass;
    long double currentRocketMass = currentFuelMass + rocketDryMass;

    long double lowerLimitEFMI = 500;
    long double higherLimitEFMI = 15000;

    std::cout << "Total distance: " << d / 1e3 << " km" << std::endl;


    long double x = 0; // Distance from Earth
    long double rocketVelocity = 0;
    long double acceleration = 0;
    long double timeExpended = 0;

    long double exhaustVelocity = 0;
    long double thrust = 0;
    long double netForce = 0;

    long double timeStep = 0.001; // in seconds

    long double currentEFMI = 3500; // Initial fuel burn rate



    sf::Font font;
    if (font.loadFromFile("assets/fonts/HomeVideo-BLG6G.ttf")) { // loading font
        std::cout << "font loaded" << std::endl;
    }
    else {
        std::cout << "font not loaded" << std::endl;
    }

    sf::Text rocketSpeedText;
    sf::Text rocketAccelerationText;
    sf::Text rocketFuelText;
    sf::Text rocketTimeText;
    sf::Text rocketDistanceText;

    sf::Text note;

    note.setFont(font);
    note.setCharacterSize(36);
    note.setFillColor(sf::Color::Red);
    note.setPosition(50, 300); // Position of the text

    rocketSpeedText.setFont(font);
    rocketSpeedText.setCharacterSize(36);
    rocketSpeedText.setFillColor(sf::Color::Black);
    rocketSpeedText.setPosition(10, 10); // Position of the text

    rocketAccelerationText.setFont(font);
    rocketAccelerationText.setCharacterSize(36);
    rocketAccelerationText.setFillColor(sf::Color::Black);
    rocketAccelerationText.setPosition(10, 40); // Position of the text

    rocketFuelText.setFont(font);
    rocketFuelText.setCharacterSize(36);
    rocketFuelText.setFillColor(sf::Color::Black);
    rocketFuelText.setPosition(10, 70); // Position of the text

    rocketTimeText.setFont(font);
    rocketTimeText.setCharacterSize(36);
    rocketTimeText.setFillColor(sf::Color::Black);
    rocketTimeText.setPosition(10, 100); // Position of the text

    rocketDistanceText.setFont(font);
    rocketDistanceText.setCharacterSize(36);
    rocketDistanceText.setFillColor(sf::Color::Black);
    rocketDistanceText.setPosition(10, 130); // Position of the text

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "simulation", sf::Style::Default);
    if (visual)
        window.close();
    // sf::CircleShape earth(50); // Earth representation
    // earth.setFillColor(sf::Color::Blue);
    // earth.setPosition(400, 300); // Position of Earth
    sf::RectangleShape rocket(sf::Vector2f(10, 50)); // Rocket representation
    rocket.setFillColor(sf::Color::Red);
    rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, window.getSize().y - rocket.getSize().y); // Initial position of the rocket

    // while (window.isOpen()) {

    // Clear the window

    // Draw the Earth and rocket
    // window.draw(earth);

    // Display the contents of the window
    // window.display();
    // sf::sleep(sf::seconds(10));

    int count = 0;

    // for (int i = 0; i <= int(210 / timeStep); i++) { // Run for 5 minutes
    while (rocketVelocity < 13000) { // Run for 5 minutes

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        // std::cout << i << std::endl;
        if (currentFuelMass > 0) {
            // calculating thrust
            thrust = sqrt(2 * enginePower * currentEFMI);

            // Burn fuel
            currentFuelMass -= currentEFMI * timeStep;
            if (currentFuelMass < 0) {
                currentFuelMass = 0;
                currentEFMI = 0; // Stop engine if fuel runs out
                thrust = 0;
            }

            // Update rocket mass
            currentRocketMass = currentFuelMass + rocketDryMass;
        }

        // Correct net force calculation
        netForce = thrust
            - ((G * massEarth * currentRocketMass) / pow((radiusEarth + x), 2))
            + ((G * massMoon * currentRocketMass) / pow((radiusMoon + d - x), 2))
            - ((G * massSun * currentRocketMass) / pow((S + x + radiusEarth), 2));

        // Acceleration update
        acceleration = netForce / currentRocketMass;
        if (acceleration >= 50) {
            currentEFMI -= 10;
        }

        // Correct motion equations
        long double distanceInTimeStep = rocketVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        rocketVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        timeExpended += 1;
        count++;

        if (visual) {
            if (count % 50 == 0 && window.isOpen()) {
                // Update rocket position for visualization
                window.clear(sf::Color::Cyan);

                rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * (1 - (x / 1020000))); // Initial position of the rocket
                rocketSpeedText.setString("Rocket Speed: " + std::to_string(rocketVelocity) + " m/s");
                rocketAccelerationText.setString("Rocket Acceleration: " + std::to_string(acceleration) + " m/s^2");
                rocketFuelText.setString("Rocket Fuel: " + std::to_string(currentFuelMass) + " kg");
                rocketTimeText.setString("Rocket Time: " + std::to_string(timeExpended * timeStep) + " seconds");
                rocketDistanceText.setString("Rocket Distance: " + std::to_string(x / 1e3) + " km");

                note.setString("Liftoff!");
                window.draw(note); // Draw the note text

                window.draw(rocketSpeedText); // Draw the rocket speed text
                window.draw(rocketAccelerationText); // Draw the rocket acceleration text
                window.draw(rocketFuelText); // Draw the rocket fuel text
                window.draw(rocketTimeText); // Draw the rocket time text
                window.draw(rocketDistanceText); // Draw the rocket distance text
                window.draw(rocket);

                window.display();
                // std::cout << i << std::endl;
            }
        }
    }

    timeExpended *= timeStep;

    std::cout << "------------------------------------------------Stage 1 Complete----------------------------------------------" << std::endl;
    // std::cout << "Stage 1 Complete" << std::endl;
    std::cout << "Time: " << timeExpended << " seconds\n";
    std::cout << "Final velocity: " << rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << x / 1e3 << " km\n";
    std::cout << "Distance Left: " << (d - x) / 1e3 << "km" << std::endl;
    std::cout << "Current Rocket Mass: " << currentRocketMass << " kg\n";
    std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";
    // std::cout << "Exhaust velocity: " << exhaustVelocity << " m/s\n";
    // std::cout << "Final thrust: " << thrust << " N\n";

    // ----------------------------------------------------------------------------------------------------------------------------
    // stage 2 cruise
    double cruiseDuration = 3600 * 12 + 60 * 34; // 12.5 hours
    double cruiseVelocity = rocketVelocity;
    double distanceCruised = 0;
    timeStep = 1;

    double distanceToCruise = d - x; // Distance to cruise

    timeExpended = 0;
    // for (int i = 0; i < cruiseDuration; i++) { // 30 hours for reference
    while (distanceCruised + 300 * 1e3 < distanceToCruise && cruiseVelocity > 0) { // 30 hours for reference
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        netForce =
            ((G * massMoon * currentRocketMass) / pow((radiusMoon + d - x), 2))
            - ((G * massEarth * currentRocketMass) / pow((radiusEarth + x), 2));
        acceleration = netForce / currentRocketMass;
        long double distanceInTimeStep = cruiseVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        cruiseVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        distanceCruised += distanceInTimeStep;
        timeExpended += 1;

        if (visual) {
            // Update rocket position for visualization
            if (window.isOpen()) {
                window.clear(sf::Color::Cyan);

                rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * (1 - (x / d))); // Initial position of the rocket
                rocketSpeedText.setString("Rocket Speed: " + std::to_string(cruiseVelocity) + " m/s");
                rocketAccelerationText.setString("Rocket Acceleration: " + std::to_string(acceleration) + " m/s^2");
                rocketFuelText.setString("Rocket Fuel: " + std::to_string(currentFuelMass) + " kg");
                rocketTimeText.setString("Rocket Time: " + std::to_string(timeExpended * timeStep) + " seconds");
                rocketDistanceText.setString("Rocket Distance: " + std::to_string(x / 1e3) + " km");

                note.setString("Cruiseing!");
                window.draw(note); // Draw the note text

                window.draw(rocketSpeedText); // Draw the rocket speed text
                window.draw(rocketAccelerationText); // Draw the rocket acceleration text
                window.draw(rocketFuelText); // Draw the rocket fuel text
                window.draw(rocketTimeText); // Draw the rocket time text
                window.draw(rocketDistanceText); // Draw the rocket distance text
                window.draw(rocket);

                window.display();
            }
        }

    }
    // std::cout << acceleration << std::endl;

    std::cout << "------------------------------------------------Stage 2 Complete----------------------------------------------" << std::endl;
    // std::cout << "Stage 2 Complete" << std::endl;
    std::cout << "Time: " << timeExpended << " seconds\n";
    std::cout << "Cruise velocity: " << cruiseVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceCruised / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (x) / 1e3 << " km\n";

    std::cout << "Distance Left: " << (d - x) / 1e3 << "km" << std::endl;;
    // std::cout << "------------------------------------------------Stage 3 Complete----------------------------------------------" << std::endl;


    // ! apply brakesssssssssssssssssssssssssssssssssssssssssss
    int totalDistanceCovered = x;

    currentEFMI = 0; // determine this
    double decelerationDuration = 120; // in seconds

    timeExpended = 0;
    thrust = 0;
    netForce = 0;
    double distanceDecelerated = 0;
    timeStep = 0.001; // in seconds
    rocketVelocity = cruiseVelocity; // Reset velocity to cruise velocity
    double distancetoDecelerate = d - x; // Distance to decelerate

    count = 0;
    // for (int i = 0; i <= int(decelerationDuration / timeStep); i++) { // Run for 5 
    while (distancetoDecelerate - 1 * 1e3 > distanceDecelerated && rocketVelocity > 0) {


        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // if (count * timeStep > 120) {
            //     currentEFMI = 5000; // determine this
            // }


        if (currentFuelMass > 0) {
            // calculating thrust
            thrust = -sqrt(2 * enginePower * currentEFMI); // Negative thrust for braking

            // Burn fuel
            currentFuelMass -= currentEFMI * timeStep;
            if (currentFuelMass < 0) {
                currentFuelMass = 0;
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
        // std::cout << acceleration << std::endl;

        // Correct motion equations
        long double distanceInTimeStep = rocketVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        rocketVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        distanceDecelerated += distanceInTimeStep;
        timeExpended += 1;

        if (count % 100 == 0) {
            if (-(pow(rocketVelocity, 2) - pow(10, 2)) / (2 * (distancetoDecelerate - 1 * 1e3 - distanceDecelerated)) < acceleration) { // 10 meter per second is the goal velocity
                currentEFMI += 10;
            }
            else {
                currentEFMI -= 20;
            }
            // currentEFMI += 10;
            if (acceleration < -50) {
                currentEFMI -= 20;
                // break;
            }
        }

        count++;
        // if (count % 1000 == 0) {
        //     // std::cout << acceleration << std::endl;
        //     std::cout << "Distance left: " << (d - x) / 1e3 << " Velocity: " << rocketVelocity << " acceleration: " << acceleration << " EMFI: " << currentEFMI << " required: " << (pow(rocketVelocity, 2) - pow(10, 2)) / (2 * (distancetoDecelerate - 1 * 1e3 - distanceDecelerated)) << std::endl;
        // }

        if (visual) {
            if (count % 10 == 0 && window.isOpen()) {
                window.clear(sf::Color::Cyan);

                rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * ((x - totalDistanceCovered) / (d - totalDistanceCovered))); // Initial position of the rocket
                rocketSpeedText.setString("Rocket Speed: " + std::to_string(rocketVelocity) + " m/s");
                rocketAccelerationText.setString("Rocket Acceleration: " + std::to_string(acceleration) + " m/s^2");
                rocketFuelText.setString("Rocket Fuel: " + std::to_string(currentFuelMass) + " kg");
                rocketTimeText.setString("Rocket Time: " + std::to_string(timeExpended * timeStep) + " seconds");
                rocketDistanceText.setString("Rocket Distance: " + std::to_string(x / 1e3) + " km");

                note.setString("decelerating!");
                window.draw(note); // Draw the note text

                window.draw(rocketSpeedText); // Draw the rocket speed text
                window.draw(rocketAccelerationText); // Draw the rocket acceleration text
                window.draw(rocketFuelText); // Draw the rocket fuel text
                window.draw(rocketTimeText); // Draw the rocket time text
                window.draw(rocketDistanceText); // Draw the rocket distance text
                window.draw(rocket);

                window.display();
            }
        }
    }
    timeExpended *= timeStep;
    // std::cout << count << std::endl;

    std::cout << "------------------------------------------------Stage 3 Complete----------------------------------------------" << std::endl;
    // std::cout << "Stage 2 Complete" << std::endl;
    std::cout << "Time: " << timeExpended << " seconds\n";
    std::cout << "Cruise velocity: " << rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (x) / 1e3 << " km\n";
    std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";

    std::cout << "Distance Left: " << (d - x) << "m" << std::endl;;

    // std::cout << "-------------------------------------------------------------------------------------------------------------" << std::endl;

    totalDistanceCovered = x;

    currentEFMI = 370; // determine this
    decelerationDuration = 4.4; // in seconds

    thrust = 0;
    netForce = 0;
    distanceDecelerated = 0;
    timeStep = 0.001; // in seconds
    // rocketVelocity = cruiseVelocity; // Reset velocity to cruise velocity

    count = 0;
    for (int i = 0; i <= int(decelerationDuration / timeStep); i++) { // Run for 5 minutes

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (currentFuelMass > 0) {
            // calculating thrust
            thrust = -sqrt(2 * enginePower * currentEFMI); // Negative thrust for braking

            // Burn fuel
            currentFuelMass -= currentEFMI * timeStep;
            if (currentFuelMass < 0) {
                currentFuelMass = 0;
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
        // std::cout << acceleration << std::endl;

        // Correct motion equations
        long double distanceInTimeStep = rocketVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        rocketVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        distanceDecelerated += distanceInTimeStep;
        timeExpended += 1;

        if (visual) {
            if (window.isOpen()) {
                window.clear(sf::Color::Cyan);

                rocket.setPosition(window.getSize().x / 2 - rocket.getSize().x / 2, (window.getSize().y - rocket.getSize().y) * ((x - totalDistanceCovered) / (d - totalDistanceCovered))); // Initial position of the rocket
                rocketSpeedText.setString("Rocket Speed: " + std::to_string(rocketVelocity) + " m/s");
                rocketAccelerationText.setString("Rocket Acceleration: " + std::to_string(acceleration) + " m/s^2");
                rocketFuelText.setString("Rocket Fuel: " + std::to_string(currentFuelMass) + " kg");
                rocketTimeText.setString("Rocket Time: " + std::to_string(timeExpended * timeStep) + " seconds");
                rocketDistanceText.setString("Rocket Distance: " + std::to_string(x / 1e3) + " km");

                note.setString("Landing!");
                window.draw(note); // Draw the note text

                window.draw(rocketSpeedText); // Draw the rocket speed text
                window.draw(rocketAccelerationText); // Draw the rocket acceleration text
                window.draw(rocketFuelText); // Draw the rocket fuel text
                window.draw(rocketTimeText); // Draw the rocket time text
                window.draw(rocketDistanceText); // Draw the rocket distance text
                window.draw(rocket);
                sf::sleep(sf::milliseconds(1)); // Sleep for 1 millisecond to control the frame rate

                window.display();
            }
        }
    }








    std::cout << "------------------------------------------------Stage 4 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << decelerationDuration << " seconds\n";
    std::cout << "Cruise velocity: " << rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (x) / 1e3 << " km\n";
    std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";

    std::cout << "Distance Left: " << (d - x) << "m" << std::endl;;

    if (visual) {
        if (window.isOpen()) {
            sf::sleep(sf::seconds(5)); // Sleep for 5 seconds to allow the user to see the final state
            window.close();
        }
    }

    return 0;
}
