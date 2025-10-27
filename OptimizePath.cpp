// File: OptimizePath.cpp
// Description: Reads a list of (X,Y,Z) points, optimizes their traversal order
//              to minimize path length (simple nearest-neighbor heuristic),
//              and displays both original and optimized paths using ROOT.
//
// Usage:
//   ./OptimizePath input.csv output.csv
//
// The program creates a ROOT canvas showing the paths and writes the optimized
// sequence to the output CSV file.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <numeric>   // for std::iota
#include <limits>

#include "Points.h"  // from ../common

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TStyle.h"

using namespace std;

//------------------------------------------------------------------------------
// Compute total length of a path given point order
//------------------------------------------------------------------------------
double computePathLength(const vector<Point>& pts, const vector<int>& order) {
    double total = 0.0;
    for (size_t i = 1; i < order.size(); ++i) {
        double dx = pts[order[i]].coords[0] - pts[order[i - 1]].coords[0];
        double dy = pts[order[i]].coords[1] - pts[order[i - 1]].coords[1];
        double dz = pts[order[i]].coords[2] - pts[order[i - 1]].coords[2];
        total += sqrt(dx * dx + dy * dy + dz * dz);
    }
    return total;
}

//------------------------------------------------------------------------------
// Simple greedy nearest-neighbor path optimization
//------------------------------------------------------------------------------
vector<int> optimizePath(const vector<Point>& pts) {
    size_t n = pts.size();
    vector<int> remaining(n);
    iota(remaining.begin(), remaining.end(), 0);

    vector<int> order;
    order.reserve(n);

    int current = 0;
    order.push_back(current);
    remaining.erase(remaining.begin());

    while (!remaining.empty()) {
        double bestDist = numeric_limits<double>::max();
        size_t bestIdx = 0;
        for (size_t i = 0; i < remaining.size(); ++i) {
            double dx = pts[remaining[i]].coords[0] - pts[current].coords[0];
            double dy = pts[remaining[i]].coords[1] - pts[current].coords[1];
            double dz = pts[remaining[i]].coords[2] - pts[current].coords[2];
            double d = sqrt(dx * dx + dy * dy + dz * dz);
            if (d < bestDist) {
                bestDist = d;
                bestIdx = i;
            }
        }
        current = remaining[bestIdx];
        order.push_back(current);
        remaining.erase(remaining.begin() + bestIdx);
    }

    return order;
}

//------------------------------------------------------------------------------
// Write points in specified order to CSV
//------------------------------------------------------------------------------
void writeReorderedPoints(const string& outFile, const vector<Point>& pts, const vector<int>& order) {
    ofstream out(outFile);
    if (!out.is_open()) {
        cerr << "Error: cannot open output file " << outFile << endl;
        exit(1);
    }
    for (int idx : order) {
        out << pts[idx].coords[0] << ","
            << pts[idx].coords[1] << ","
            << pts[idx].coords[2] << "\n";
    }
    cout << "Wrote reordered points to " << outFile << endl;
}

//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------
int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " input.csv output.csv" << endl;
        return 1;
    }

    string inFile = argv[1];
    string outFile = argv[2];

    // Initialize ROOT GUI
    TApplication app("OptimizePathApp", &argc, argv);

    // Read points
    vector<Point> pts = readPoints(inFile, 3);
    if (pts.empty()) {
        cerr << "Error: no points read from " << inFile << endl;
        return 1;
    }

    // Compute initial path
    vector<int> origOrder(pts.size());
    iota(origOrder.begin(), origOrder.end(), 0);
    double origLen = computePathLength(pts, origOrder);

    // Optimize
    vector<int> optOrder = optimizePath(pts);
    double optLen = computePathLength(pts, optOrder);

    cout << "Initial path length = " << origLen << endl;
    cout << "Optimized path length = " << optLen << endl;

    // Write reordered points
    writeReorderedPoints(outFile, pts, optOrder);

    // Create graph for original and optimized paths
    TCanvas* c1 = new TCanvas("c1", "Path Optimization", 1000, 700);
    gStyle->SetOptStat(0);

    TGraph* gOrig = new TGraph(pts.size());
    TGraph* gOpt = new TGraph(pts.size());
    for (size_t i = 0; i < pts.size(); ++i) {
        gOrig->SetPoint(i, pts[origOrder[i]].coords[0], pts[origOrder[i]].coords[1]);
        gOpt->SetPoint(i, pts[optOrder[i]].coords[0], pts[optOrder[i]].coords[1]);
    }

    gOrig->SetLineColor(kRed);
    gOrig->SetLineWidth(2);
    gOrig->SetMarkerStyle(20);
    gOrig->SetTitle("Original Path;X;Y");

    gOpt->SetLineColor(kBlue);
    gOpt->SetLineWidth(2);
    gOpt->SetMarkerStyle(21);

    gOrig->Draw("ALP");
    gOpt->Draw("LP SAME");

    // Add legend
    TLegend* leg = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg->AddEntry(gOrig, "Original Path", "lp");
    leg->AddEntry(gOpt, "Optimized Path", "lp");
    leg->Draw();

    app.Run();  // Keeps the plot window open
    return 0;
}
