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
#include "Point.h"
using namespace std;



// For testing data input -- print a few
void printSamplePoints(const vector<Point>& points, size_t sampleSize) {
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < sampleSize; i++) {
        cout << "Point " << i + 1 << ": ";
        cout << points[i].x << " " << points[i].y << " " << points[i].z << " " << points[i].r << " " 
                << points[i].g << " " << points[i].b << " " << points[i].source;
        cout << endl;
    }
}

    
// Function to convert a point to a comma-separated string, for output use
string pointToString(const Point& point) {
    ostringstream oss;
    oss << point.x << ", " << point.y << ", " << point.z << ", " << point.r << ", " << point.g << ", " << point.b << "," << point.source << ",";
    return oss.str();
}


// Parsing functions for each data types:
// Function to parse csv or txt line
Point parseLine(const string& line, char delimiter, string source) {
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


vector<Point> parsePlyData(ifstream& file, int vertexCount, string source) {
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



vector<Point> parseLasData(const string& filePath, string source) {
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

// End of Parsing functions





// Read input point cloud data
vector<Point> readPointCloudData(const string& filePath) {
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



int main(int argc, char *argv[]) {

    vector<Point> points; // point cloud data points

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




/*  Multiple construction tests
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

    /*  Normal 1 time Construction

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
    
    */

    /*    
    // Create a random number generator engine
    random_device rd;  // Seed the random number generator
    mt19937 mt(rd());  // Mersenne Twister 19937 generator
    uniform_int_distribution<int> dist(0, points.size()-1);

    // Test K-NN search on 10 random points
    for (int queryIndex = 0; queryIndex < 10; queryIndex++) {
        // Generate a random query point
        int randomIndex = dist(mt);
        Point queryPoint = points[randomIndex];

        string queryPointStr = pointToString(queryPoint);
        // cout << "Query Point: " << queryPointStr << endl;

        for (int k = 1; k < 101; k++) {
            // Start timer for k-NN search
            auto start2 = chrono::high_resolution_clock::now();

            // Repeat the k-NN search multiple times
            for (int repeat = 0; repeat < 1000; ++repeat) {
                vector<Point> nearestNeighbors = kdTree.kNearestNeighbors(queryPoint, k);
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
    Point queryPoint = points[0];
    string queryPointStr = pointToString(queryPoint);

    cout << "Query Point: " << queryPointStr << endl;
    for (int k = 1; k < 11; k++) {
            // Start timer for k-NN search
            auto start2 = chrono::high_resolution_clock::now();

            // Repeat the k-NN search multiple times
            for (int repeat = 0; repeat < 100; ++repeat) {
                vector<Point> nearestNeighbors = kdTree.kNearestNeighbors(queryPoint, k);
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
