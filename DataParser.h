#include <vector>
#include <string>
#include "Point.h" 

class DataParser {
public:
    static vector<Point> readPointCloudData(const string& filePath);   // Take PLY, LAS, CSV or TXT input.

private:
    // Helper parsing functions
    static Point parseLine(const string& line, char delimiter, string source);
    static vector<Point> parsePlyData(ifstream& file, int vertexCount, string source);
    static vector<Point> parseLasData(const string& filePath, string source);
};


