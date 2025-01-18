#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>

struct Table1Entry {
    std::vector<std::string_view> Bs;
    std::vector<std::string_view> Cs;
};

void parseLine(const std::string& line, char delimiter, std::string_view& part1, std::string_view& part2) {
    size_t delimPos = line.find(delimiter);
    if (delimPos != std::string::npos) {
        part1 = std::string_view(line.data(), delimPos);
        part2 = std::string_view(line.data() + delimPos + 1, line.size() - delimPos - 1);
    }
}

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    // Use string_view consistently as the key type
    std::unordered_map<std::string_view, Table1Entry> table1; // Key: A, Values: Bs and Cs
    table1.reserve(20000000);

    std::unordered_map<std::string_view, std::vector<std::string_view>> table3; // Key: A, Values: D's
    table3.reserve(20000000);

    std::unordered_map<std::string_view, std::vector<std::string_view>> table4; // Key: D, Values: E's
    table4.reserve(20000000);

    // Vectors to store lines for lifetime safety of string_views
    std::vector<std::string> file1Lines, file2Lines, file3Lines, file4Lines;

    // Read File1 (A,B)
    std::ifstream file1(path1);
    if (!file1.is_open()) {
        std::cerr << "Error opening File1\n";
        return -1;
    }
    std::string line;
    std::string_view A, B;
    while (std::getline(file1, line)) {
        file1Lines.push_back(std::move(line)); // Store the line
        const std::string& storedLine = file1Lines.back(); // Reference the stored line
        parseLine(storedLine, ',', A, B);
        table1[A].Bs.push_back(B); // Use string_view
    }

    // Read File2 (A,C)
    std::ifstream file2(path2);
    if (!file2.is_open()) {
        std::cerr << "Error opening File2\n";
        return -1;
    }
    std::string_view C;
    while (std::getline(file2, line)) {
        file2Lines.push_back(std::move(line)); // Store the line
        const std::string& storedLine = file2Lines.back();
        parseLine(storedLine, ',', A, C);
        table1[A].Cs.push_back(C); // Use string_view
    }

    // Read File3 (A,D)
    std::ifstream file3(path3);
    if (!file3.is_open()) {
        std::cerr << "Error opening File3\n";
        return -1;
    }
    std::string_view D;
    while (std::getline(file3, line)) {
        file3Lines.push_back(std::move(line)); // Store the line
        const std::string& storedLine = file3Lines.back();
        parseLine(storedLine, ',', A, D);
        table3[A].push_back(D); // Use string_view
    }

    // Read File4 (D,E)
    std::ifstream file4(path4);
    if (!file4.is_open()) {
        std::cerr << "Error opening File4\n";
        return -1;
    }
    std::string_view E;
    while (std::getline(file4, line)) {
        file4Lines.push_back(std::move(line)); // Store the line
        const std::string& storedLine = file4Lines.back();
        parseLine(storedLine, ',', D, E);
        table4[D].push_back(E); // Use string_view
    }

    // Perform the join
    for (const auto& [A, entry] : table1) {
        auto it3 = table3.find(A);
        if (it3 != table3.end()) {
            for (const auto& D : it3->second) {
                auto it4 = table4.find(D);
                if (it4 != table4.end()) {
                    for (const auto& E : it4->second) {
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
