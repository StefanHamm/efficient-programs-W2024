#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

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

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1; // Key: A, Values: Bs and Cs
    table1.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table3; // Key: A, Values: D's
    table3.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table4; // Key: D, Values: E's
    table4.reserve(20000000);


    
    // Read File1 (A,B)
    std::ifstream file1(path1);
    if (!file1.is_open()) {
        std::cerr << "Error opening File1\n";
        return -1;
    }
    std::string line, A, B;
    while (std::getline(file1, line)) {
        parseLine(line, ',', A, B);
        table1[A].Bs.push_back(B);
    }

    // Read File2 (A,C)
    std::ifstream file2(path2);
    if (!file2.is_open()) {
        std::cerr << "Error opening File2\n";
        return -1;
    }
    std::string C;
    while (std::getline(file2, line)) {
        parseLine(line, ',', A, C);
        table1[A].Cs.push_back(C);
    }

    // Read File3 (A,D)
    std::ifstream file3(path3);
    if (!file3.is_open()) {
        std::cerr << "Error opening File3\n";
        return -1;
    }
    std::string D;
    while (std::getline(file3, line)) {
        parseLine(line, ',', A, D);
        table3[A].push_back(D);
    }

    // Read File4 (D,E)
    std::ifstream file4(path4);
    if (!file4.is_open()) {
        std::cerr << "Error opening File4\n";
        return -1;
    }
    std::string E;
    while (std::getline(file4, line)) {
        parseLine(line, ',', D, E);
        table4[D].push_back(E);
    }
// Perform the join
    for (const auto& [A, entry] : table1) {
    auto itTable3 = table3.find(A);
    if (itTable3 != table3.end()) {
        const auto& Ds = itTable3->second;
        for (const auto& D : Ds) {
            auto itTable4 = table4.find(D);
            if (itTable4 != table4.end()) {
                const auto& Es = itTable4->second;
                // Unroll the loops for Bs and Cs (assuming even number of elements in each)
                size_t sizeB = entry.Bs.size();
                size_t sizeC = entry.Cs.size();

                // Unrolling Bs and Cs
                for (size_t i = 0; i < sizeB; i += 2) {
                    // Process two elements from Bs
                    for (size_t j = 0; j < sizeC; j += 2) {
                        std::cout << D << "," << A << "," << entry.Bs[i] << "," << entry.Cs[j] << "," << Es[0] << std::endl;
                        if (i + 1 < sizeB && j + 1 < sizeC) {
                            std::cout << D << "," << A << "," << entry.Bs[i + 1] << "," << entry.Cs[j + 1] << "," << Es[0] << std::endl;
                        }
                    }
                }

                // Fallback if the size of Bs or Cs is odd
                if (sizeB % 2 != 0 || sizeC % 2 != 0) {
                    for (size_t i = sizeB - (sizeB % 2); i < sizeB; i++) {
                        for (size_t j = sizeC - (sizeC % 2); j < sizeC; j++) {
                            std::cout << D << "," << A << "," << entry.Bs[i] << "," << entry.Cs[j] << "," << Es[0] << std::endl;
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