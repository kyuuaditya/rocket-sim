#include <sfml/Graphics.hpp>
#include <iostream>
#include <math.h>

int main() {
    double massEarth = 5.972e24; // Mass of Earth in kg
    double radiusEarth = 6.378e6; // Radius of Earth in m

    double massMoon = 7.347e22; // Mass of Moon in kg
    double radiusMoon = 1.7374e6; // Radius of Moon in m

    double d = 3.850006e8 - radiusEarth - radiusMoon;

    double G = 6.674e-11; // gravitational constant

    double enginePower = 1.0e12; // engine power in watts

    double rocketTotalMass = 1.0e6; // rocket mass in kg
    double rocketDryMass = 2.0e5; // dry mass in kg
    double initialFuelMass = rocketTotalMass - rocketDryMass; // fuel mass in kg
    double currentFuelMass = initialFuelMass; // current fuel mass in kg
    double currnetRocketMass = currentFuelMass + rocketDryMass;

    double lowerLimitEFMI = 2; // engine fuel mass input in kg/s
    double higherLimitEFMI = 20000;

    double currentEFMI = 0; // ! engine fuel mass input

    double x = 0; // how far the rocket is from earth

    bool engineON = true;

    double exhaustVelocity = 0;
    double thrust = 0; // rocket thrust 

    double timeStep = 0.1;

    double netForce = 0;


    if (engineON && currentEFMI != 0) {
        // exhaustVelocity = sqrt(2 * enginePower / (currentEFMI));
        // thrust = 2 * enginePower / exhaustVelocity;
        // currentFuelMass -= currentEFMI;
        // currnetRocketMass = currentFuelMass + rocketDryMass;

        // netForce = thrust - ((G * massEarth * currnetRocketMass) / pow((radiusEarth + x), 2)) + ((G * massMoon * currnetRocketMass) / pow((radiusMoon + d - x), 2));


        // std::cout << netForce << std::endl;
        // std::cout << netForce / currnetRocketMass << std::endl;
        // std::cout << (initialFuelMass / currentEFMI) / 60 << std::endl;
    }

    currentEFMI = 1000;
    currentEFMI = (double)currentEFMI * timeStep;

    double rocketVelocity = 0;
    double acceleration = 0;
    double timeExpended = 0;
    double distanceInTimeStep = 0;

    // std::cout << currentEFMI << std::endl;
    for (double i = 0;i <= 60;i += timeStep) {
        timeExpended += timeStep;
        exhaustVelocity = (double)sqrt(2 * (enginePower * timeStep) / currentEFMI);
        thrust = (double)2 * (enginePower * timeStep) / exhaustVelocity;
        currentFuelMass -= (double)currentEFMI;
        currnetRocketMass = (double)currentFuelMass + rocketDryMass;

        netForce = (double)thrust - ((G * massEarth * currnetRocketMass) / pow((radiusEarth + x), 2)) * timeStep + ((G * massMoon * currnetRocketMass) / pow((radiusMoon + d - x), 2)) * timeStep;
        acceleration = netForce / currnetRocketMass;
        std::cout << "Exhaust velocity: " << exhaustVelocity << " Thrust: " << thrust << " CurrentFuelMass: " << currentFuelMass << " CurrentRocketMass: " << currnetRocketMass << " NetForce: " << netForce << " Acceleration: " << acceleration << std::endl;

        // std::cout << netForce << " " << netForce / currnetRocketMass << std::endl;
        distanceInTimeStep = rocketVelocity * timeStep + (1 / 2) * acceleration * pow(acceleration, 2);
        x += distanceInTimeStep;
        rocketVelocity = rocketVelocity + acceleration * timeStep;
    }

    std::cout << "currentFuelMass: " << currentFuelMass << " rocketVelocity: " << rocketVelocity << " distanceTravelled: " << x << "m timeExpended: " << timeExpended << std::endl;
}