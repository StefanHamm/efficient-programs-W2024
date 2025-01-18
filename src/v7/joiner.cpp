#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>

// Struct to store Bs and Cs for table1
struct Table1Entry {
    std::vector<std::string> Bs;
    std::vector<std::string> Cs;
};

// Utility function to parse lines efficiently
void parseLine(const std::string& line, char delimiter, std::string_view& part1, std::string_view& part2) {
    size_t delimPos = line.find(delimiter);
    if (delimPos != std::string::npos) {
        part1 = std::string_view(line.data(), delimPos);  // Use string_view to avoid copying
        part2 = std::string_view(line.data() + delimPos + 1, line.size() - delimPos - 1); // Use string_view
    }
}

// Optimized hash join implementation
int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    // Hash tables
    std::unordered_map<std::string, Table1Entry> table1;
    std::unordered_map<std::string, std::vector<std::string>> table3;
    std::unordered_map<std::string, std::vector<std::string>> table4;

    // Reserve memory upfront to minimize rehashing
    table1.reserve(20000000);
    table3.reserve(20000000);
    table4.reserve(20000000);

    // Read File1 (A,B)
    std::ifstream file1(path1);
    if (!file1.is_open()) {
        std::cerr << "Error opening File1\n";
        return -1;
    }
    std::string line;
    std::string_view A, B;
    while (std::getline(file1, line)) {
        parseLine(line, ',', A, B);
        auto& entry = table1[A];
        entry.Bs.emplace_back(B);  // Use move semantics to avoid unnecessary copies
    }

    // Read File2 (A,C)
    std::ifstream file2(path2);
    if (!file2.is_open()) {
        std::cerr << "Error opening File2\n";
        return -1;
    }
    std::string_view C;
    while (std::getline(file2, line)) {
        parseLine(line, ',', A, C);
        auto& entry = table1[A];
        entry.Cs.emplace_back(C);  // Use move semantics
    }

    // Read File3 (A,D)
    std::ifstream file3(path3);
    if (!file3.is_open()) {
        std::cerr << "Error opening File3\n";
        return -1;
    }
    std::string_view D;
    while (std::getline(file3, line)) {
        parseLine(line, ',', A, D);
        table3[A].emplace_back(D);
    }

    // Read File4 (D,E)
    std::ifstream file4(path4);
    if (!file4.is_open()) {
        std::cerr << "Error opening File4\n";
        return -1;
    }
    std::string_view E;
    while (std::getline(file4, line)) {
        parseLine(line, ',', D, E);
        table4[D].emplace_back(E);
    }

    // Perform the join
    std::ostringstream outputBuffer; // Buffer to batch output
    for (const auto& [A, entry] : table1) {
        auto table3It = table3.find(A);
        if (table3It == table3.end()) continue; // Skip if no matches in table3

        for (const auto& D : table3It->second) {
            auto table4It = table4.find(D);
            if (table4It == table4.end()) continue; // Skip if no matches in table4

            for (const auto& E : table4It->second) {
                for (const auto& B : entry.Bs) {
                    for (const auto& C : entry.Cs) {
                        outputBuffer << D << "," << A << "," << B << "," << C << "," << E << "\n";
                    }
                }
            }
        }
    }

    // Flush the buffer to stdout
    std::cout << outputBuffer.str();
    return 0;
}

int join(const std::string path1, const std::string path2, const std::string path3, const std::string path4) {
    return hashJoin(path1, path2, path3, path4);
}
