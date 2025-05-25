#pragma once
#include <fstream>

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
};