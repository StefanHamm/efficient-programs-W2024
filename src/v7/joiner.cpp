#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

struct Table1Entry {
    std::vector<std::string> Bs;
    std::vector<std::string> Cs;
};

// Efficient line parsing without creating unnecessary strings
void parseLine(const std::string& line, char delimiter, std::string& part1, std::string& part2) {
    size_t delimPos = line.find(delimiter);
    if (delimPos != std::string::npos) {
        part1 = line.substr(0, delimPos);  // Avoid extra allocations
        part2 = line.substr(delimPos + 1);
    }
}

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1; // Key: A, Values: Bs and Cs
    table1.reserve(20000000);  // Reserve sufficient space upfront

    std::unordered_map<std::string, std::vector<std::string>> table3; // Key: A, Values: D's
    table3.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table4; // Key: D, Values: E's
    table4.reserve(20000000);

    // File parsing logic using buffered reads to minimize overhead
    std::ifstream file1(path1);
    if (!file1.is_open()) {
        std::cerr << "Error opening File1\n";
        return -1;
    }
    std::string line, A, B;
    while (std::getline(file1, line)) {
        parseLine(line, ',', A, B);
        table1[A].Bs.push_back(std::move(B)); // Move instead of copying
    }

    std::ifstream file2(path2);
    if (!file2.is_open()) {
        std::cerr << "Error opening File2\n";
        return -1;
    }
    std::string C;
    while (std::getline(file2, line)) {
        parseLine(line, ',', A, C);
        table1[A].Cs.push_back(std::move(C)); // Move instead of copying
    }

    std::ifstream file3(path3);
    if (!file3.is_open()) {
        std::cerr << "Error opening File3\n";
        return -1;
    }
    std::string D;
    while (std::getline(file3, line)) {
        parseLine(line, ',', A, D);
        table3[A].push_back(std::move(D)); // Move instead of copying
    }

    std::ifstream file4(path4);
    if (!file4.is_open()) {
        std::cerr << "Error opening File4\n";
        return -1;
    }
    std::string E;
    while (std::getline(file4, line)) {
        parseLine(line, ',', D, E);
        table4[D].push_back(std::move(E)); // Move instead of copying
    }

    // Perform the join with reduced redundant lookups
    for (auto& [A, entry] : table1) {
        auto it3 = table3.find(A); // Find once and store the result
        if (it3 != table3.end()) {
            auto& DList = it3->second; // Avoid looking up `A` repeatedly
            for (const auto& D : DList) {
                auto it4 = table4.find(D); // Find D once for each D in table3
                if (it4 != table4.end()) {
                    auto& EList = it4->second; // Avoid looking up D repeatedly
                    for (const auto& E : EList) {
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
    return hashJoin(path1, path2, path3, path4);
}
