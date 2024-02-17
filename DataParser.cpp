#include "DataParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

// LibLAS Library
#include <liblas/liblas.hpp>      
#include <liblas/reader.hpp>
#include <liblas/factory.hpp>
#include <liblas/point.hpp>
#include <liblas/color.hpp>

using namespace std;


// Parsing functions for each data types:
// Function to parse csv or txt line
Point DataParser::parseLine(const string& line, char delimiter, string source) {
    istringstream lineStream(line);
    string value;
    vector<float> point;

    // Read all components into the vector
    while (getline(lineStream, value, delimiter)) {
        point.push_back(stof(value));
    }

    // Validation
    if (point.size() < 6) {
        throw runtime_error("Invalid data: Each line must contain at least attributes for XYZRGB.");
    }

    // Extract the first 3 attributes for x, y, z and the last 3 for r, g, b
    float x = point[0];
    float y = point[1];
    float z = point[2];
    int r = static_cast<int>(point[point.size() - 3]);  
    int g = static_cast<int>(point[point.size() - 2]);  
    int b = static_cast<int>(point[point.size() - 1]);  

    // Construct and return a Point with these values
    return Point(x, y, z, r, g, b, source);
}


vector<Point> DataParser::parsePlyData(ifstream& file, int vertexCount, string source) {
    vector<Point> points;

    for (int i = 0; i < vertexCount; ++i) {
        float x, y, z;
        uint8_t r, g, b;

        file.read(reinterpret_cast<char*>(&x), sizeof(float));
        file.read(reinterpret_cast<char*>(&y), sizeof(float));
        file.read(reinterpret_cast<char*>(&z), sizeof(float));
        file.read(reinterpret_cast<char*>(&r), sizeof(uint8_t));
        file.read(reinterpret_cast<char*>(&g), sizeof(uint8_t));
        file.read(reinterpret_cast<char*>(&b), sizeof(uint8_t));

        points.emplace_back(x, y, z, r, g, b, source);
    }

    return points;
}



vector<Point> DataParser::parseLasData(const string& filePath, string source) {
    vector<Point> points;
    ifstream ifs(filePath, ios::in | ios::binary);

    if (!ifs.is_open()) {
        throw runtime_error("Error opening LAS file: " + filePath);
    }

    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);

    while (reader.ReadNextPoint()) {
        const liblas::Point& p = reader.GetPoint();
        const liblas::Color& color = p.GetColor();

        points.emplace_back(p.GetX(), p.GetY(), p.GetZ(), color.GetRed(), color.GetGreen(), color.GetBlue(), source);
    }

    ifs.close();
    return points;
}

// End of Helper Parsing functions



// Main Parsing function - Read input point cloud data
vector<Point> DataParser::readPointCloudData(const string& filePath) {
    vector<Point> points;
    string line;
    string fileExtension = filePath.substr(filePath.find_last_of(".") + 1);
    string fileName = filePath.substr(filePath.find_last_of("/") + 1); 


    if (fileExtension == "csv") {           // handle csv file
        ifstream file(filePath);
        if (!file.is_open()) {
            throw runtime_error("Error opening the file: " + filePath);
        }

        getline(file, line); // Skip header
        while (getline(file, line)) {
            points.push_back(parseLine(line, ',', fileName));
        }
        file.close();
    } 

    else if (fileExtension == "txt") {    // handle txt file
        ifstream file(filePath);
        if (!file.is_open()) {
            throw runtime_error("Error opening the file: " + filePath);
        }

        while (getline(file, line)) {
            points.push_back(parseLine(line, ' ', fileName));
        }
        file.close();
    } 

    else if (fileExtension == "ply") {
        ifstream file(filePath, ios::in | ios::binary);
        
        getline(file, line); // Skip "ply"
        getline(file, line); // Skip "format binary_little_endian 1.0"
        getline(file, line); // Read "element vertex <count>"

        int vertexCount = stoi(line.substr(line.find_last_of(' ') + 1)); // get vertex

        // Skip the rest of the header
        do {
            getline(file, line);
        } while (line != "end_header");
        getline(file, line);

        points = parsePlyData(file, vertexCount, fileName);
        file.close();
    } 

    else if (fileExtension == "las") {
        points = parseLasData(filePath, fileName);
    }
    
    else {
        throw runtime_error("Unsupported file type: " + fileExtension);
    }

    return points;
}