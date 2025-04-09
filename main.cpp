#include <iostream>
#include <cmath>

int main() {
    const long double massEarth = 5.972e24; // kg
    const long double radiusEarth = 6.378e6; // meters

    const long double massMoon = 7.347e22; // kg
    const long double radiusMoon = 1.7374e6; // meters

    const long double d = 3.844006e8 - radiusEarth - radiusMoon; // Distance between Earth & Moon minus their radii
    const long double G = 6.674e-11; // gravitational constant

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
    for (int i = 0; i <= int(210 / timeStep); i++) { // Run for 5 minutes
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
            + ((G * massMoon * currentRocketMass) / pow((radiusMoon + d - x), 2));

        // Acceleration update
        acceleration = netForce / currentRocketMass;

        // Correct motion equations
        long double distanceInTimeStep = rocketVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        rocketVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        timeExpended += 1;
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

    timeExpended = 0;
    for (int i = 0; i < cruiseDuration; i++) { // 30 hours for reference
        netForce =
            ((G * massMoon * currentRocketMass) / pow((radiusMoon + d - x), 2))
            - ((G * massEarth * currentRocketMass) / pow((radiusEarth + x), 2));
        acceleration = netForce / currentRocketMass;
        long double distanceInTimeStep = cruiseVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
        cruiseVelocity += acceleration * timeStep;
        x += distanceInTimeStep;
        distanceCruised += distanceInTimeStep;
        timeExpended += 1;
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

    currentEFMI = 550; // determine this
    double decelerationDuration = 120; // in seconds

    thrust = 0;
    netForce = 0;
    double distanceDecelerated = 0;
    timeStep = 0.001; // in seconds
    rocketVelocity = cruiseVelocity; // Reset velocity to cruise velocity

    int count = 0;
    for (int i = 0; i <= int(decelerationDuration / timeStep); i++) { // Run for 5 minutes

        if (count * timeStep > 120) {
            currentEFMI = 5000; // determine this
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

        count++;
        // if (count % 60 == 0) {
            // std::cout << "Distance left: " << (d - x) / 1e3 << " Velocity: " << rocketVelocity << " acceleration: " << acceleration << std::endl;
        // }
    }
    // timeExpended *= timeStep;
    // std::cout << count << std::endl;

    std::cout << "------------------------------------------------Stage 3 Complete----------------------------------------------" << std::endl;
    // std::cout << "Stage 2 Complete" << std::endl;
    std::cout << "Time: " << decelerationDuration << " seconds\n";
    std::cout << "Cruise velocity: " << rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (x) / 1e3 << " km\n";
    std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";

    std::cout << "Distance Left: " << (d - x) / 1e3 << "km" << std::endl;;

    // std::cout << "-------------------------------------------------------------------------------------------------------------" << std::endl;


    currentEFMI = 370; // determine this
    decelerationDuration = 4.4; // in seconds

    thrust = 0;
    netForce = 0;
    distanceDecelerated = 0;
    timeStep = 0.001; // in seconds
    // rocketVelocity = cruiseVelocity; // Reset velocity to cruise velocity

    count = 0;
    for (int i = 0; i <= int(decelerationDuration / timeStep); i++) { // Run for 5 minutes

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

        count++;
        if (count % 100 == 0) {
            // std::cout << "Distance left: " << (d - x) / 1e3 << " Velocity: " << rocketVelocity << " acceleration: " << acceleration << std::endl;
        }
    }








    std::cout << "------------------------------------------------Stage 4 Complete----------------------------------------------" << std::endl;
    std::cout << "Time: " << decelerationDuration << " seconds\n";
    std::cout << "Cruise velocity: " << rocketVelocity << " m/s\n";
    std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
    std::cout << "Total Distance traveled: " << (x) / 1e3 << " km\n";
    std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";

    std::cout << "Distance Left: " << (d - x) / 1e3 << "km" << std::endl;;

    return 0;
}
