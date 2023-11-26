#pragma once

#include <vector>
#include <queue>
using namespace std;


class KdTree {
    public:
        struct Node {
            vector<double> point;
            Node* left;
            Node* right;
            
            Node(vector<double>& input) : point(input), left(nullptr), right(nullptr) {}
        };

        KdTree();

        ~KdTree() {
            destroyTree(root);
        }

        // Disable copy constructor and copy assignment operator
        KdTree(const KdTree&) = delete;
        KdTree& operator=(const KdTree&) = delete;

        // Define move constructor and move assignment operator
        KdTree(KdTree&& other) noexcept : root(other.root) {
            other.root = nullptr;
        }

        KdTree& operator=(KdTree&& other) noexcept {
            if (this != &other) {
                destroyTree(root);
                root = other.root;
                other.root = nullptr;
            }
            return *this;
        }

        // Function to build the KdTree
        void buildTree(vector<vector<double>>& points);

        // Function to perform k-NN search
        vector<vector<double>> kNearestNeighbors(vector<double>& queryPoint, int k);

    private:
        Node* root;

        void destroyTree(Node* node) {
            if (node) {
                destroyTree(node->left);
                destroyTree(node->right);
                delete node;
            }
        }


        Node* buildTreeRecursive (vector<vector<double>> &input, int depth);

        // Helper function for k-NN search
        void kNearestNeighborsRecursive(Node* currentNode, vector<double>& queryPoint, int k,
                                    priority_queue<pair<double, vector<double>>>& nearestNeighbors, int depth);

};