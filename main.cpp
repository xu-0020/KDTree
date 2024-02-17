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
#include "DataParser.h"
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

string pointToString(const Point& point) {
    ostringstream oss;
    oss << fixed << setprecision(2) << point.x << ", " << point.y << ", " << point.z << ", "
        << point.r << ", " << point.g << ", " << point.b << ", " << point.source;
    return oss.str();
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
       points = DataParser::readPointCloudData(filePath);
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

    // Write CSV headers
    outFile << "X,Y,Z,R,G,B,Source" << endl;
    // Write point data
    for (const auto& point : points) {
        outFile << pointToString(point) << endl;
    }


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
