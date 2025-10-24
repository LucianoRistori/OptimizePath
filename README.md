OptimizePath

Version: v0.8.0
Author: Luciano Ristori
Date: October 2025

Overview

OptimizePath is a C++ utility designed to optimize the order of measured points on a surface — for example, points acquired with a Coordinate Measurement Machine (CMM) — by finding an efficient traversal path that minimizes total travel distance in the XY plane.

The program implements a nearest-neighbor heuristic to approximate the Traveling Salesman Problem (TSP). It reads a CSV file containing point labels and XYZ coordinates, computes the optimized visiting order, and writes the sorted list to a new output file using the same CSV format.

This tool can be used to plan or analyze scanning paths, especially in surface-flatness or alignment measurements.

Features

Reads points using the shared readPoints() function from common/.

Interprets coordinates as (X, Y, Z), but only uses X and Y for path optimization.

Preserves labels in input and output files.

Outputs a CSV with points sorted in optimal visiting order.

Reports total path length before and after optimization, with reduction percentage.

Usage

./OptimizePath input.csv output.csv

Example:

Input file (input.csv):
P1,10.0,10.0,0.5
P2,20.0,5.0,0.6
P3,15.0,25.0,0.4

Output file (output.csv):
P2,20.0,5.0,0.6
P1,10.0,10.0,0.5
P3,15.0,25.0,0.4

Terminal output:
Total path length before optimization: 34.12
Total path length after optimization: 26.85
Reduction: 21.3 %
Sorted points written to output.csv

Build Instructions

Requires ROOT for visualization (planned for v0.9.0).
For v0.8.0, no ROOT linkage is yet required.

Example Makefile target:

clang++ -std=c++17 -O2 -Wall OptimizePath.cpp ../common/Points.cpp
-o OptimizePath -stdlib=libc++ -I../common

Version History

Version | Date | Notes
v0.8.0 | Oct 2025 | First stable version — path optimization and CSV output
(next: v0.9.0) | (planned) | Add ROOT visualization of before/after paths

Repository Structure

OptimizePath/
├── OptimizePath.cpp (Main source)
├── Makefile (Build rules)
├── README.md (Documentation)
└── ../common/Points.* (Shared library for point management)

License

MIT License — see top-level repository for details.