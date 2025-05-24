#pragma once

class Constants {
public:
    const long double massEarth = 5.972e24; // kg
    const long double radiusEarth = 6.378e6; // meters

    const long double massSun = 1.989e30; // kg
    const long double radiusSun = 6.963e8; // meters

    const long double massMoon = 7.347e22; // kg
    const long double radiusMoon = 1.737e6; // meters

    const long double distanceEarthMoon = 3.84400e8; // distance between Earth and Moon (center to center)
    const long double distanceEarthSun = 1.496e11; // distance between Earth and Sun

    const long double GravitationalConstant = 6.674e-11; // gravitational constant

    const long double GapEarthMoon = distanceEarthMoon - radiusEarth - radiusMoon;

};