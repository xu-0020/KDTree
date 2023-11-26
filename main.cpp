#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>

#include "KdTree.h"
using namespace std;

    
// Function to convert a point to a comma-separated string
string pointToString(const vector<double>& point) {
    ostringstream oss;
    for (size_t i = 0; i < point.size(); ++i) {
        if (i != 0) {
            oss << ", ";
        }
        oss << point[i];
    }
    return oss.str();
}




int main(int argc, char *argv[]) {

    vector<vector<double>> points;  // use vector of vectors to store point clouds
    string line;

    // Handle input data
    ifstream file("Point_cloud1.csv");
    
    if (!file.is_open()) {
        cerr << "Error opening the file." << endl;
        return 1;
    }
    
    
    // Read and display the header
    if (getline(file, line)) {
        cout << "Header: " << line << endl;
    }

    // Acquire data
    while (getline(file, line)) {
        vector<double> point;
        istringstream lineStream(line);
        string value;

        
        if (getline(lineStream, value, ',')) {
            double coord = stod(value);
            point.push_back(coord);
        }
        
        points.push_back(point);
    }

    file.close();




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

    

    outFile.close();

    return 0;
}
