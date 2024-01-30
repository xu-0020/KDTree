#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <cstdlib>
#include <iomanip>    // for std::setprecision

// LibLAS Library
#include <liblas/liblas.hpp>      
#include <liblas/reader.hpp>
#include <liblas/factory.hpp>
#include <liblas/point.hpp>
#include <liblas/color.hpp>


#include "KdTree.h"
using namespace std;


// For testing data input -- print a few
void printSamplePoints(const vector<vector<double>>& points, size_t sampleSize) {
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < sampleSize; i++) {
        cout << "Point " << i + 1 << ": ";
        for (double val : points[i]) {
            cout << val << " ";
        }
        cout << endl;
    }
}

    
// Function to convert a point to a comma-separated string, for output use
string pointToString(const vector<double>& point) {
    ostringstream oss;
    for (size_t i = 0; i < point.size(); i++) {
        if (i != 0) {
            oss << ", ";
        }
        oss << point[i];
    }
    return oss.str();
}



// Parsing functions for each data types:
// Function to parse csv or txt line
vector<double> parseLine(const string& line, char delimiter) {
    vector<double> point;
    istringstream lineStream(line);
    string value;

    while (getline(lineStream, value, delimiter)) {
        try {
            point.push_back(stod(value));
        } catch (const invalid_argument&) {
            cerr << "Warning: Invalid number '" << value << "' found." << endl;
        }
    }

    return point;
}

// Define a struct to represent a point from a PLY file
struct PlyData {
    float x, y, z; // 3d attributes
    uint8_t red, green, blue; // color

    vector<double> toVector() const {
        return {static_cast<double>(x), 
            static_cast<double>(y), 
            static_cast<double>(z), 
            static_cast<double>(red), 
            static_cast<double>(green), 
            static_cast<double>(blue), 
        };
    }
};

// Function to parse the PLY file
vector<vector<double>> parsePlyData(ifstream& file, int vertexCount) {
    vector<vector<double>> points;
    PlyData point;

    for (int i = 0; i < vertexCount; i++) {
        file.read(reinterpret_cast<char*>(&point), sizeof(PlyData));
        points.push_back(point.toVector());
    }

    return points;
}

vector<vector<double>> parseLasData(const string& filePath) {
    vector<vector<double>> points;

    ifstream ifs;
    ifs.open(filePath, ios::in | ios::binary);

    if (!ifs.is_open()) {
        throw runtime_error("Error opening LAS file: " + filePath);
    }

    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);

    while (reader.ReadNextPoint()) {
        const liblas::Point& p = reader.GetPoint();
        const liblas::Color& color = p.GetColor();

        // Extract X, Y, Z and RGB
        vector<double> point = {
            static_cast<double>(p.GetX()), 
            static_cast<double>(p.GetY()), 
            static_cast<double>(p.GetZ()),
            static_cast<double>(color.GetRed()),
            static_cast<double>(color.GetGreen()),
            static_cast<double>(color.GetBlue()),
        };
        points.push_back(point);
    }

    return points;
}

// End of Parsing functions





// Read input point cloud data
vector<vector<double>> readPointCloudData(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        throw runtime_error("Error opening the file: " + filePath);
    }

    vector<vector<double>> points;
    string line;
    string fileExtension = filePath.substr(filePath.find_last_of(".") + 1);

    if (fileExtension == "csv") {           // handle csv file
        getline(file, line); // Skip header
        while (getline(file, line)) {
            points.push_back(parseLine(line, ','));
        }

    } 
    else if (fileExtension == "txt") {    // handle txt file
        getline(file, line); // Skip header
        while (getline(file, line)) {
            points.push_back(parseLine(line, ' '));
        }

    } 
    else if (fileExtension == "ply") {
        getline(file, line); // Skip "ply"
        getline(file, line); // Skip "format binary_little_endian 1.0"
        getline(file, line); // Read "element vertex <count>"

        int vertexCount = stoi(line.substr(line.find_last_of(' ') + 1)); // get vertex

        // Skip the rest of the header
        do {
            getline(file, line);
        } while (line != "end_header");

        points = parsePlyData(file, vertexCount);
    } 
    else if (fileExtension == "las") {
        points = parseLasData(filePath);
    }
    else {
        throw runtime_error("Unsupported file type: " + fileExtension);
    }

    file.close();

    return points;
}



int main(int argc, char *argv[]) {

    vector<vector<double>> points; // point cloud data points


    // Guide user to enter input file path
    string filePath;
    cout << "Please enter point cloud data file path: " << endl;
    cin >> filePath;
    // "Data/Point_cloud1.csv"


    // Read point cloud data
    try {
       points = readPointCloudData(filePath);
    }
    catch (exception e) {
        cerr << "IOException" << endl;
        return 1;
    }

    printSamplePoints(points, 10);

    // Prepare Output
    ofstream outFile("Output.csv");
    if (!outFile.is_open()) {
        cerr << "Error opening output file." << endl;
        return 1;
    }
    outFile << "Query Point,K-Value,Time Taken (nanoseconds)" << endl;


/*  
    // Start the timer for construction
    auto start1 = chrono::high_resolution_clock::now();

    for (int i=0; i<10; i++) {
         // Call build tree function
        KdTree kdTree;
        kdTree.buildTree(points);
    }


    // Stop the timer for construction
    auto stop1 = chrono::high_resolution_clock::now();

    // Calculate construction duration
    auto duration1 = chrono::duration_cast<chrono::seconds>(stop1 - start1) / 10;
    cout << "Time for building the tree: " << duration1.count() << " seconds" << endl;

*/



   // Start the timer for construction
    auto start1 = chrono::high_resolution_clock::now();

    // Call build tree function
    KdTree kdTree;
    kdTree.buildTree(points);

    // Stop the timer for construction
    auto stop1 = chrono::high_resolution_clock::now();

    // Calculate construction duration
    auto duration1 = chrono::duration_cast<chrono::seconds>(stop1 - start1);
    cout << "Time for building the tree: " << duration1.count() << " seconds" << endl;
    


    /*    
    // Create a random number generator engine
    random_device rd;  // Seed the random number generator
    mt19937 mt(rd());  // Mersenne Twister 19937 generator
    uniform_int_distribution<int> dist(0, points.size()-1);

    // Test K-NN search on 10 random points
    for (int queryIndex = 0; queryIndex < 10; queryIndex++) {
        // Generate a random query point
        int randomIndex = dist(mt);
        vector<double> queryPoint = points[randomIndex];

        string queryPointStr = pointToString(queryPoint);
        // cout << "Query Point: " << queryPointStr << endl;

        for (int k = 1; k < 101; k++) {
            // Start timer for k-NN search
            auto start2 = chrono::high_resolution_clock::now();

            // Repeat the k-NN search multiple times
            for (int repeat = 0; repeat < 1000; ++repeat) {
                vector<vector<double>> nearestNeighbors = kdTree.kNearestNeighbors(queryPoint, k);
            }

            auto stop2 = chrono::high_resolution_clock::now();
            auto duration2 = chrono::duration_cast<chrono::nanoseconds>(stop2 - start2) / 1000;
            // cout << "K-value: " << k << ", Average time taken: " << duration2.count() << " nanoseconds" << endl;
            
            if (k == 1) {
                outFile << "\"" << queryPointStr << "\",";  
            }
            else {
                outFile << "" << ",";
            }
            // Write to the CSV file
            outFile << k << "," << duration2.count() << endl;
        }
    }

    */

    /*
    vector<double> queryPoint = points[0];
    string queryPointStr = pointToString(queryPoint);

    cout << "Query Point: " << queryPointStr << endl;
    for (int k = 1; k < 11; k++) {
            // Start timer for k-NN search
            auto start2 = chrono::high_resolution_clock::now();

            // Repeat the k-NN search multiple times
            for (int repeat = 0; repeat < 100; ++repeat) {
                vector<vector<double>> nearestNeighbors = kdTree.kNearestNeighbors(queryPoint, k);
            }

            auto stop2 = chrono::high_resolution_clock::now();
            auto duration2 = chrono::duration_cast<chrono::nanoseconds>(stop2 - start2) / 100;
            cout << "K-value: " << k << ", Average time taken: " << duration2.count() << " nanoseconds" << endl;
            
            if (k == 1) {
                outFile << "\"" << queryPointStr << "\",";  
            }
            else {
                outFile << "" << ",";
            }
            // Write to the CSV file
            outFile << k << "," << duration2.count() << endl;
        }
    
    */

    outFile.close();

    return 0;
}
