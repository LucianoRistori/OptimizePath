# OptimizePath

**Version 0.9.0 – October 2025**

OptimizePath is a C++ utility designed to optimize the order of measured points on a surface — for example, points acquired with a Coordinate Measurement Machine (CMM) — by finding an efficient traversal path that minimizes total travel distance in the XY plane.

The program implements a nearest-neighbor heuristic to approximate the Traveling Salesman Problem (TSP). It reads a CSV file containing point labels and XYZ coordinates, computes the optimized visiting order, and writes the sorted list to a new output file using the same CSV format.

This tool can be used to plan or analyze scanning paths, especially in surface-flatness or alignment measurements.

---

## Features

- Reads points using the shared `readPoints()` function from `../common/`.
- Interprets coordinates as (X, Y, Z); path optimization uses all three coordinates for length calculation, but plots are shown in the XY plane.
- Preserves **labels** in both input and output files.
- Outputs a CSV with points sorted in optimal visiting order.
- Reports total path length **before and after optimization**, and reduction percentage.
- Displays **three interactive ROOT canvases**:
  1. **Original Path** (red)
  2. **Optimized Path** (blue)
  3. **Both paths superimposed** for direct comparison.
- Uses a **modular structure** with the shared `Points.h` and `Points.cpp` in `../common`.
- Cleaned Makefile automatically links ROOT libraries using `root-config`.

---

## Usage

```bash
./OptimizePath input.csv output.csv
```

### Example

**Input file (`input.csv`):**
```
P1,10.0,10.0,0.5
P2,20.0,5.0,0.6
P3,15.0,25.0,0.4
```

**Output file (`output.csv`):**
```
P2,20.0,5.0,0.6
P1,10.0,10.0,0.5
P3,15.0,25.0,0.4
```

**Terminal output:**
```
Initial path length = 34.12
Optimized path length = 26.85
Reduction: 21.3 %
Wrote reordered points (with labels) to output.csv
```

Three ROOT windows will open:
1. Original path  
2. Optimized path  
3. Both paths superimposed (red and blue)

---

## Build Instructions

ROOT must be initialized in your environment (`source thisroot.sh` or equivalent).

```bash
make clean
make
```

### Requirements
- macOS or Linux with `clang++` or `g++`
- ROOT (≥ 6.28)
- Shared `../common` directory containing:
  - `Points.h`
  - `Points.cpp`

### Example Makefile Target (simplified excerpt)
```makefile
clang++ -std=c++17 -O2 -Wall OptimizePath.cpp ../common/Points.cpp \
    -o OptimizePath -stdlib=libc++ -I../common $(shell root-config --cflags --libs)
```

---

## Version History

| Version | Date | Notes |
|----------|------|-------|
| **v0.9.0** | Oct 2025 | Added three-canvas ROOT visualization, label preservation, and cleaned Makefile |
| **v0.8.0** | Oct 2025 | First stable version — path optimization and CSV output |

---

## Repository Structure

```
OptimizePath/
├── OptimizePath.cpp   # Main source
├── Makefile           # Build rules (ROOT-enabled)
├── README.md          # Documentation
└── ../common/
    ├── Points.h
    └── Points.cpp     # Shared library for point management
```

---

## License

MIT License — see top-level repository for details.
