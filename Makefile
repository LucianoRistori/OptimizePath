# ===============================================================
# Makefile for OptimizePath
# Author: Luciano Ristori
# Updated: October 2025
#
# Description:
#   Builds the OptimizePath program using the shared common/ module.
#   Implements heuristic optimization of CMM measurement order
#   (Traveling Salesman-style problem) with ROOT visualization.
# ===============================================================

# ---- Compiler and Flags ----
CXX      := clang++
CXXFLAGS := -O2 -Wall -Wextra -Wno-cpp -stdlib=libc++ -mmacosx-version-min=13.0 \
            -std=c++17 -pthread -m64
INCLUDES := -I../common $(shell root-config --cflags)
LIBS     := $(shell root-config --libs) -Wl,-rpath,$(shell root-config --libdir) \
            -lm -ldl -lpthread

# ---- Targets ----
TARGET := OptimizePath
OBJS   := OptimizePath.o ../common/Points.o

# ---- Default Rule ----
all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(INCLUDES) $(LIBS)

# ---- Compilation Rules ----
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

../common/Points.o: ../common/Points.cpp ../common/Points.h
	@echo "Compiling common/Points.cpp..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c ../common/Points.cpp -o ../common/Points.o

# ---- Cleaning ----
clean:
	@echo "Cleaning up..."
	rm -f *.o ../common/Points.o $(TARGET)
	find . -name "*.dSYM" -type d -exec rm -rf {} +

# ---- Convenience ----
rebuild: clean all

.PHONY: all clean rebuild
