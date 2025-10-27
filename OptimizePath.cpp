// ============================================================================
// File: OptimizePath.cpp
// Author: Luciano Ristori
// Created: October 2025
// Version: 0.9.0
//
// Description:
//   This program reads a list of labeled 3D points (X, Y, Z) — typically
//   coordinates from a CMM scan or similar measurement set — and determines
//   an optimized traversal order that minimizes the total path length between
//   consecutive points.
//
//   The optimization is based on a simple greedy nearest-neighbor heuristic,
//   which provides a fast but non-global approximation to the optimal
//   Traveling Salesman path.  The resulting order is useful for minimizing
//   travel time or repositioning movements in scanning or machining systems.
//
//   The program outputs:
//     • Console summary of the original and optimized path lengths
//     • A reordered CSV file containing the optimized sequence
//     • Three interactive ROOT canvases:
//         1. Original path (red)
//         2. Optimized path (blue)
//         3. Both paths superimposed for visual comparison
//
// Usage:
//   ./OptimizePath input.csv output.csv
//
// Input format (CSV or space-separated):
//   label,X,Y,Z
//   or
//   X,Y,Z               (label optional)
//
// Output format:
//   label,X,Y,Z         (in optimized order)
//
// Dependencies:
//   • ROOT framework (for visualization)
//   • Points.h / Points.cpp from ../common (defines Point structure and I/O)
//
// Compilation:
//   Handled by the provided Makefile using root-config for automatic linking.
//   Example:
//       make clean && make
//
// Notes:
//   - The algorithm is deterministic and assumes the first point as the start.
//   - Path lengths are computed in 3D Euclidean space.
//   - The program is intended for exploratory analysis, visualization, and
//     workflow optimization, not for rigorous combinatorial minimization.
//
// ============================================================================


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
        out << pts[idx].label << ","
            << pts[idx].coords[0] << ","
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

   //------------------------------------------------------------------------------
// Visualization: three separate canvases for original, optimized, and comparison
//------------------------------------------------------------------------------
gStyle->SetOptStat(0);

// --- 1. Original path ---
TCanvas* c1 = new TCanvas("c1", "Original Path", 800, 600);
(void)c1;
TGraph* gOrig = new TGraph(pts.size());
for (size_t i = 0; i < pts.size(); ++i)
    gOrig->SetPoint(i, pts[origOrder[i]].coords[0], pts[origOrder[i]].coords[1]);
gOrig->SetLineColor(kRed);
gOrig->SetLineWidth(2);
gOrig->SetMarkerStyle(20);
gOrig->SetTitle("Original Path;X;Y");
gOrig->Draw("ALP");

TLegend* leg1 = new TLegend(0.75, 0.82, 0.92, 0.9);
leg1->AddEntry(gOrig, "Original Path", "lp");
leg1->Draw();

// --- 2. Optimized path ---
TCanvas* c2 = new TCanvas("c2", "Optimized Path", 800, 600);
(void)c2;
TGraph* gOpt = new TGraph(pts.size());
for (size_t i = 0; i < pts.size(); ++i)
    gOpt->SetPoint(i, pts[optOrder[i]].coords[0], pts[optOrder[i]].coords[1]);
gOpt->SetLineColor(kBlue);
gOpt->SetLineWidth(2);
gOpt->SetMarkerStyle(21);
gOpt->SetTitle("Optimized Path;X;Y");
gOpt->Draw("ALP");

TLegend* leg2 = new TLegend(0.75, 0.82, 0.92, 0.9);
leg2->AddEntry(gOpt, "Optimized Path", "lp");
leg2->Draw();

// --- 3. Comparison: both paths superimposed ---
TCanvas* c3 = new TCanvas("c3", "Comparison: Original vs Optimized", 900, 700);
(void)c3;
TGraph* gOrig2 = new TGraph(*gOrig); // copy
TGraph* gOpt2  = new TGraph(*gOpt);

gOrig2->SetLineColor(kRed);
gOrig2->SetLineWidth(2);
gOrig2->SetMarkerStyle(20);
gOrig2->SetTitle("Original (Red) vs Optimized (Blue);X;Y");

gOpt2->SetLineColor(kBlue);
gOpt2->SetLineWidth(2);
gOpt2->SetMarkerStyle(21);

gOrig2->Draw("ALP");
gOpt2->Draw("LP SAME");

TLegend* leg3 = new TLegend(0.7, 0.8, 0.9, 0.9);
leg3->AddEntry(gOrig2, "Original Path", "lp");
leg3->AddEntry(gOpt2, "Optimized Path", "lp");
leg3->Draw();

// --- Update and run interactive session ---
c1->Update();
c2->Update();
c3->Update();

app.Run();  // Keeps all canvases open

    return 0;
}
