#include "KdTree.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

KdTree::KdTree() : root(nullptr) {};

// Build tree from root
void KdTree::buildTree(vector<vector<double>> &input) { 
    root = buildTreeRecursive(input, 0); 
}


// Retrieves the k nearest neighbors; returns them in order from nearest to farthest
vector<vector<double>> KdTree::kNearestNeighbors(vector<double>& queryPoint, int k) {
    priority_queue<pair<double, vector<double>> > nearestNeighbors; // max-heap to store neighbours
    kNearestNeighborsRecursive(root, queryPoint, k, nearestNeighbors, 0);

    vector<vector<double>> result;
    while (!nearestNeighbors.empty()) {
        result.push_back(nearestNeighbors.top().second);
        nearestNeighbors.pop();
    }
    reverse(result.begin(), result.end());
    return result;
}


// Extend tree nodes
KdTree::Node* KdTree::buildTreeRecursive (vector<vector<double>> &input, int depth) {
    if (input.empty()) {
        return nullptr;
    }

    int dimensions = input[0].size();
    int axis = depth % dimensions;  // select the current tree level dimension cyclical

    // Sort points based on the current axis
    sort(input.begin(), input.end(), [axis](vector<double>& a, vector<double>& b) {
        return a[axis] < b[axis];
    });

    // pick out the current split node
    int median = input.size() / 2;
    Node* medianPoint = new Node(input[median]);

    // Create non-const vectors for recursive calls
    vector<vector<double>> leftInput(input.begin(), input.begin() + median);
    vector<vector<double>> rightInput(input.begin() + median + 1, input.end());

    medianPoint->left = buildTreeRecursive(leftInput, depth + 1);
    medianPoint->right = buildTreeRecursive(rightInput, depth + 1);

    return medianPoint;
}



void KdTree::kNearestNeighborsRecursive(Node* currentNode, vector<double>& queryPoint, int k,
                                      priority_queue<pair<double, vector<double>>>& nearestNeighbors, int depth) {
    if (currentNode == nullptr) {
        return;
    }

    int dimensions = queryPoint.size();
    int axis = depth % dimensions;

    // Calculate squared distance (euclidean) for better efficiency
    double squaredDistance = 0;
    for (int i = 0; i < dimensions; i++) {
        squaredDistance += pow(currentNode->point[i] - queryPoint[i], 2);
    }
    nearestNeighbors.push({squaredDistance, currentNode->point});

    // remove further point
    if (nearestNeighbors.size() > k) {
        nearestNeighbors.pop();
    }

    // Decide which subtree to explore
    double axisDist = queryPoint[axis] - currentNode->point[axis];
    Node* nextNode = axisDist < 0 ? currentNode->left : currentNode->right;
    Node* otherNode = axisDist < 0 ? currentNode->right : currentNode->left;

    kNearestNeighborsRecursive(nextNode, queryPoint, k, nearestNeighbors, depth + 1);

    // Check if we need to explore the other side
    if (nearestNeighbors.size() < k || pow(axisDist, 2) < nearestNeighbors.top().first) {
        kNearestNeighborsRecursive(otherNode, queryPoint, k, nearestNeighbors, depth + 1);
    }
}

