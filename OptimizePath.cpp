//===============================================================
// File: OptimizePath.cpp
// Purpose: Optimize travel path using XY coordinates from Points,
//          output original lines reordered according to best path.
//===============================================================
//
// Usage:
//   ./OptimizePath input.txt output.txt
//
//===============================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <string>
#include <iomanip>
#include "Points.h"  // uses your existing readPoints()

using namespace std;

//---------------------------------------------------------------
// Compute total XY path length
//---------------------------------------------------------------
double pathLength(const vector<Point>& pts, const vector<int>& order) {
    double total = 0.0;
    for (size_t i = 1; i < order.size(); ++i) {
        double dx = pts[order[i]].coords[0] - pts[order[i - 1]].coords[0];
        double dy = pts[order[i]].coords[1] - pts[order[i - 1]].coords[1];
        total += sqrt(dx * dx + dy * dy);
    }
    return total;
}

//---------------------------------------------------------------
// 2-opt optimization (minimize XY path length)
//---------------------------------------------------------------
void twoOpt(const vector<Point>& pts, vector<int>& order) {
    bool improved = true;
    while (improved) {
        improved = false;
        double best = pathLength(pts, order);
        for (size_t i = 1; i + 2 < order.size(); ++i) {
            for (size_t j = i + 1; j + 1 < order.size(); ++j) {
                vector<int> newOrder = order;
                reverse(newOrder.begin() + i, newOrder.begin() + j + 1);
                double d = pathLength(pts, newOrder);
                if (d < best - 1e-9) { // small tolerance
                    order.swap(newOrder);
                    best = d;
                    improved = true;
                }
            }
        }
    }
}

//---------------------------------------------------------------
// Main
//---------------------------------------------------------------
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " input.txt output.txt\n";
        return 1;
    }

    string infile  = argv[1];
    string outfile = argv[2];

    //--- Read numeric points (handles CSV, spaces, etc.)
    vector<Point> pts = readPoints(infile);
    size_t n = pts.size();
    if (n < 2) {
        cerr << "Error: less than 2 points found in " << infile << endl;
        return 1;
    }

    //--- Re-read all lines as raw text (exact formatting)
    vector<string> rawLines;
    {
        ifstream fin(infile);
        if (!fin.is_open()) {
            cerr << "Error: cannot reopen input file " << infile << endl;
            return 1;
        }
        string line;
        while (getline(fin, line)) {
            if (!line.empty()) rawLines.push_back(line);
        }
    }

    if (rawLines.size() != n) {
        cerr << "Warning: mismatch between parsed points (" << n
             << ") and raw lines (" << rawLines.size() << ")\n";
    }

    //--- Initial order
    vector<int> order(n);
    iota(order.begin(), order.end(), 0);

    cout << "Initial path length = " << pathLength(pts, order) << endl;
    twoOpt(pts, order);
    cout << "Optimized path length = " << pathLength(pts, order) << endl;

    //--- Write reordered original lines
    ofstream out(outfile);
    if (!out.is_open()) {
        cerr << "Error: cannot write " << outfile << endl;
        return 1;
    }

    for (int idx : order) {
        if (idx < (int)rawLines.size())
            out << rawLines[idx] << "\n";
    }

    cout << "Wrote reordered lines to " << outfile << endl;
    return 0;
}
