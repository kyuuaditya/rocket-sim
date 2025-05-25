#pragma once
#include <fstream>
#include <iostream>

class printData {
public:
    // Function to write data to a CSV file
    std::ofstream file;

    printData() : file("filename.csv") {}

    void writeDataToCSV(double a, double b, double c, bool write) {
        if (write) {
            if (file.is_open()) {
                file << a << " " << b << " " << c << "\n";
            }
            else {
                std::cerr << "Failed to open file" << std::endl;
            }
        }
    }

    void print_to_terminal(int phase = 0, long double timeExpended = 0, long double rocketVelocity = 0, long double netDisplacement = 0, long double GapEarthMoon = 0, long double currentRocketMass = 0, long double initialFuelMass = 0, long double currentFuelMass = 0, long double distanceCruised = 0, long double distanceDecelerated = 0) {
        if (phase == 0) {
            std::cout << "Phase Time: " << timeExpended << " seconds\n";
            std::cout << "Final velocity: " << rocketVelocity << " m/s\n";
            std::cout << "Distance traveled: " << netDisplacement / 1e3 << " km\n";
            std::cout << "Distance Left: " << (GapEarthMoon - netDisplacement) / 1e3 << "km" << std::endl;
            std::cout << "Current Rocket Mass: " << currentRocketMass << " kg\n";
            std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";
        }
        else if (phase == 1) {
            std::cout << "Time: " << timeExpended << " seconds\n";
            std::cout << "Final velocity: " << rocketVelocity << " m/s\n";
            std::cout << "Distance Cruised: " << distanceCruised / 1e3 << " km\n";
            std::cout << "Total Distance traveled: " << (netDisplacement) / 1e3 << " km\n";
            std::cout << "Distance Left: " << (GapEarthMoon - netDisplacement) / 1e3 << "km" << std::endl;
        }
        else if (phase == 2 || phase == 3) {
            std::cout << "Phase Time: " << timeExpended << " seconds\n";
            std::cout << "Final velocity: " << rocketVelocity << " m/s\n";
            std::cout << "Distance traveled: " << distanceDecelerated / 1e3 << " km\n";
            std::cout << "Total Distance traveled: " << (netDisplacement) / 1e3 << " km\n";
            std::cout << "Fuel consumed: " << initialFuelMass - currentFuelMass << " kg " << "Fuel remaining: " << currentFuelMass << " kg\n";
            std::cout << "Distance Left: " << (GapEarthMoon - netDisplacement) << "m\n";
        }
    }
};