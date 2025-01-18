#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include "joiner.h"

// Chunk size (adjust based on your system's memory and performance characteristics)
const size_t CHUNK_SIZE = 1024 * 1024 * 1000; //  MB

struct Table1Entry {
    std::vector<std::string> Bs;
    std::vector<std::string> Cs;
};

void parseLine(const std::string& line, char delimiter, std::string& part1, std::string& part2) {
    size_t delimPos = line.find(delimiter);
    if (delimPos != std::string::npos) {
        part1 = line.substr(0, delimPos);
        part2 = line.substr(delimPos + 1);
    }
}

// Function to process a chunk of a file
template <typename TableType>
void processChunk(std::ifstream& file, char delimiter, TableType& table) {
    std::string line, key, value;
    while (std::getline(file, line)) {
        parseLine(line, delimiter, key, value);
        table[key].push_back(value);
    }
}

// Function to process a chunk of file1 and file2
void processChunk(std::ifstream& file1, std::ifstream& file2, char delimiter, std::unordered_map<std::string, Table1Entry>& table1, bool isFirstChunk) {
    std::string line, A, B, C;
    if (isFirstChunk) {
        // Read file1 until a full chunk is processed or EOF is reached.
        while (std::getline(file1, line)) {
            parseLine(line, ',', A, B);
            table1[A].Bs.push_back(B);
        }

        // Read file2 until a full chunk is processed or EOF is reached.
        while (std::getline(file2, line)) {
            parseLine(line, ',', A, C);
            table1[A].Cs.push_back(C);
        }
    } else {
        // Reset file streams to the beginning for subsequent chunks
        file1.clear();
        file1.seekg(0);
        file2.clear();
        file2.seekg(0);

        // Re-process to update table1
        while (std::getline(file1, line)) {
            parseLine(line, ',', A, B);
            table1[A].Bs.push_back(B);
        }

        while (std::getline(file2, line)) {
            parseLine(line, ',', A, C);
            table1[A].Cs.push_back(C);
        }
    }
}

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1;
    std::unordered_map<std::string, std::vector<std::string>> table3;
    std::unordered_map<std::string, std::vector<std::string>> table4;

    // Open all files
    std::ifstream file1(path1);
    std::ifstream file2(path2);
    std::ifstream file3(path3);
    std::ifstream file4(path4);

    if (!file1.is_open() || !file2.is_open() || !file3.is_open() || !file4.is_open()) {
        std::cerr << "Error opening files\n";
        return -1;
    }

    // Process file1 and file2 in chunks
    bool isFirstChunk = true;
    while (file1.peek() != EOF || file2.peek() != EOF) {
        processChunk(file1, file2, ',', table1, isFirstChunk);
        isFirstChunk = false;
    }

    // Process file3 and file4 in chunks
    while (file3.peek() != EOF) {
        processChunk(file3, ',', table3);
    }

    while (file4.peek() != EOF) {
        processChunk(file4, ',', table4);
    }

    // Perform the join
    for (const auto& [A, entry] : table1) {
        if (table3.find(A) != table3.end()) {
            for (const auto& D : table3[A]) {
                if (table4.find(D) != table4.end()) {
                    for (const auto& E : table4[D]) {
                        for (const auto& B : entry.Bs) {
                            for (const auto& C : entry.Cs) {
                                std::cout << D << "," << A << "," << B << "," << C << "," << E << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int join(const std::string path1, const std::string path2, const std::string path3, const std::string path4) {
    int code = hashJoin(path1, path2, path3, path4);
    return code;
}