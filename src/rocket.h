#pragma once
#include "constants.h"

class Rocket {
public:
    long double enginePower = 50.0e10;
    long double rocketTotalMass = 2.8e6; // kg
    long double rocketDryMass = 4.0e5; // kg

    long double initialFuelMass = rocketTotalMass - rocketDryMass;
    long double currentFuelMass = initialFuelMass;
    long double currentRocketMass = currentFuelMass + rocketDryMass;

    long double lowerLimitEFMI = 1;
    long double upperLimitEFMI = 20000;

    long double currentEFMI = 10000; // Initial fuel burn rate

    long double netDisplacement = 0;
    long double rocketVelocity = 0;
    long double acceleration = 0;
    long double timeExpended_m = 0;
    long double timeExpended_s = 0; // seconds
    long double exhaustVelocity = 0;
    long double thrust = 0;
    long double netForce = 0;
    long double distanceCruised = 0;
    long double distanceDecelerated = 0;

private:

public:
    void rocketBurn(long double timeStep, int phase = 0);
    void rocketMove(long double timeStep, int phase = 0);
};