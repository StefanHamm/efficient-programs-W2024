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

std::string readFileIntoString(const std::string& path) {
    std::ifstream input_file(path, std::ios::in | std::ios::binary);
    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1; // Key: A, Values: Bs and Cs
    table1.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table3; // Key: A, Values: D's
    table3.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table4; // Key: D, Values: E's
    table4.reserve(20000000);

    // Read and process File1 (A,B)
    std::string file1_contents = readFileIntoString(path1);
    std::istringstream file1_stream(file1_contents);
    std::string line, A, B;
    while (std::getline(file1_stream, line)) {
        parseLine(line, ',', A, B);
        table1[A].Bs.push_back(B);
    }

    // Read and process File2 (A,C)
    std::string file2_contents = readFileIntoString(path2);
    std::istringstream file2_stream(file2_contents);
    std::string C;
    while (std::getline(file2_stream, line)) {
        parseLine(line, ',', A, C);
        table1[A].Cs.push_back(C);
    }

    // Read and process File3 (A,D)
    std::string file3_contents = readFileIntoString(path3);
    std::istringstream file3_stream(file3_contents);
    std::string D;
    while (std::getline(file3_stream, line)) {
        parseLine(line, ',', A, D);
        table3[A].push_back(D);
    }

    // Read and process File4 (D,E)
    std::string file4_contents = readFileIntoString(path4);
    std::istringstream file4_stream(file4_contents);
    std::string E;
    while (std::getline(file4_stream, line)) {
        parseLine(line, ',', D, E);
        table4[D].push_back(E);
    }

    // Perform the join (unchanged)
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
