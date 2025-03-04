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

void parseLine(const std::string& line, char delimiter, std::string_view& part1, std::string_view& part2) {
    size_t delimPos = line.find(delimiter);
    if (delimPos != std::string::npos) {
        part1 = std::string_view(line.data(), delimPos);  // Use string_view
        part2 = std::string_view(line.data() + delimPos + 1, line.size() - delimPos - 1); // Use string_view
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
    std::string line;
    std::string_view A, B;
    while (std::getline(file1, line)) {        
        parseLine(line, ',', A, B);
        table1[std::string(A)].Bs.push_back(std::string(B));
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
        table1[std::string(A)].Cs.push_back(std::string(C));
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
        table3[std::string(A)].push_back(std::string(D));
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
        table4[std::string(D)].push_back(std::string(E));
    }

    // Perform the join
    for (const auto& [A, entry] : table1) {
        auto it3 = table3.find(A);
        if (it3 != table3.end()) {
            const auto& Ds = it3->second;
            for (const auto& D : Ds) {
                auto it4 = table4.find(D);
                if (it4 != table4.end()) {
                    const auto& Es = it4->second;
                    const auto& Bs = entry.Bs;
                    const auto& Cs = entry.Cs;
                    for (const auto& E : Es) {
                        for (const auto& B : Bs) {
                            for (const auto& C : Cs) {
                                std::cout << D << ',' << A << ',' << B << ',' << C << ',' << E << '\n';
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
