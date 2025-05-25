#include "rocket.h"
#include "cmath"
#include <iostream>

void Rocket::rocketBurn(long double timeStep, int phase) {
    if (currentFuelMass > currentEFMI * timeStep) { // check if fuel is available
        currentFuelMass -= currentEFMI * timeStep; // Burn fuel

        thrust = sqrt(2 * enginePower * currentEFMI); // calculate thrust -> movement force
        exhaustVelocity = sqrt((2 * enginePower) / currentEFMI); // calculate exhaust velocity -> for display only
        currentRocketMass = currentFuelMass + rocketDryMass; // Update rocket mass
    }
    else {
        thrust = 0; // No fuel left, no thrust
        exhaustVelocity = 0; // No exhaust velocity if no fuel
    }
}

void Rocket::rocketMove(long double timeStep, int phase) {
    Constants Constants;

    if (phase == 0 || phase == 1) {
        netForce = thrust
            - ((Constants.GravitationalConstant * Constants.massEarth * currentRocketMass) / pow((Constants.radiusEarth + netDisplacement), 2))
            + ((Constants.GravitationalConstant * Constants.massMoon * currentRocketMass) / pow((Constants.radiusMoon + Constants.GapEarthMoon - netDisplacement), 2));
    }
    else if (phase == 2 || phase == 3) {
        netForce = -thrust
            + ((Constants.GravitationalConstant * Constants.massEarth * currentRocketMass) / pow((Constants.radiusEarth + netDisplacement), 2))
            - ((Constants.GravitationalConstant * Constants.massMoon * currentRocketMass) / pow((Constants.radiusMoon + Constants.GapEarthMoon - netDisplacement), 2));
    }

    acceleration = netForce / currentRocketMass;

    long double distanceInTimeStep = rocketVelocity * timeStep + 0.5 * acceleration * pow(timeStep, 2);
    rocketVelocity += acceleration * timeStep;
    if (!thrust) distanceCruised += distanceInTimeStep;
    if (phase == 2 || phase == 3) distanceDecelerated += distanceInTimeStep; // Only add to deceleration phase if phase is 2
    netDisplacement += distanceInTimeStep;
    timeExpended_m += 1;
}