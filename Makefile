# Makefile for Efficient Programs WS24/25 - myjoin Project

# Compiler and flags
CC = g++
CFLAGS = -O3 -Wall -Wextra -std=c++17
LDFLAGS =

# Project files
SRC_DIR = src/v($(VERSION))
SRC = $(SRC_DIR)/main.cpp $(SRC_DIR)/utils.cpp $(SRC_DIR)/joiner.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = myjoin_v$(VERSION)

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJ) $(TARGET)

# Run tests
# Replace paths as needed to point to the test files on g0
TEST_SMALL_INPUT = /localtmp/efficient24/a.csv /localtmp/efficient24/b.csv /localtmp/efficient24/c.csv /localtmp/efficient24/d.csv
TEST_SMALL_OUTPUT = /localtmp/efficient24/abcd.csv
TEST_BIG_INPUT = /localtmp/efficient24/f1.csv /localtmp/efficient24/f2.csv /localtmp/efficient24/f3.csv /localtmp/efficient24/f4.csv
TEST_BIG_OUTPUT = /localtmp/efficient24/output.csv

testsmall: $(TARGET)
	./$(TARGET) $(TEST_SMALL_INPUT) | sort | diff - $(TEST_SMALL_OUTPUT)
	
testbig: $(TARGET)
	./$(TARGET) $(TEST_BIG_INPUT) | sort | diff - $(TEST_BIG_OUTPUT)

sanity: 
	./scripts/myjoin $(TEST_SMALL_INPUT) | sort | diff - $(TEST_SMALL_OUTPUT)
	./scripts/myjoin $(TEST_BIG_INPUT) | sort | diff - $(TEST_BIG_OUTPUT)

# Measure performance
perf: $(TARGET)
	LC_NUMERIC=en_US perf stat -e cycles ./$(TARGET) $(TEST_BIG_INPUT) | cat >/dev/null

perf-v: $(TARGET)
	LC_NUMERIC=en_US perf stat -e cycles ./$(TARGET) $(TEST_BIG_INPUT) | cat >/dev/null


.PHONY: all clean test perf
