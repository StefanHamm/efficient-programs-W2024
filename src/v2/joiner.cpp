#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

struct Table1Entry {
    std::unordered_set<std::string> Bs;
    std::unordered_set<std::string> Cs;
};

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1; // Key: A, Values: Bs and Cs
    std::unordered_map<std::string, std::unordered_set<std::string>> table3; // Key: A, Values: D's
    std::unordered_map<std::string, std::unordered_set<std::string>> table4; // Key: D, Values: E's

    // Read File1 (A,B)
    std::ifstream file1(path1);
    if (!file1.is_open()) {
        std::cerr << "Error opening File1\n";
        return -1;
    }
    std::string line;
    while (std::getline(file1, line)) {
        std::istringstream iss(line);
        std::string A, B;
        if (std::getline(iss, A, ',') && std::getline(iss, B)) {
            table1[A].Bs.push_back(B);
        }
    }

    // Read File2 (A,C)
    std::ifstream file2(path2);
    if (!file2.is_open()) {
        std::cerr << "Error opening File2\n";
        return -1;
    }
    while (std::getline(file2, line)) {
        std::istringstream iss(line);
        std::string A, C;
        if (std::getline(iss, A, ',') && std::getline(iss, C)) {
            table1[A].Cs.push_back(C);
        }
    }

    // Read File3 (A,D)
    std::ifstream file3(path3);
    if (!file3.is_open()) {
        std::cerr << "Error opening File3\n";
        return -1;
    }
    while (std::getline(file3, line)) {
        std::istringstream iss(line);
        std::string A, D;
        if (std::getline(iss, A, ',') && std::getline(iss, D)) {
            table3[A].push_back(D);
        }
    }

    // Read File4 (D,E)
    std::ifstream file4(path4);
    if (!file4.is_open()) {
        std::cerr << "Error opening File4\n";
        return -1;
    }
    while (std::getline(file4, line)) {
        std::istringstream iss(line);
        std::string D, E;
        if (std::getline(iss, D, ',') && std::getline(iss, E)) {
            table4[D].push_back(E);
        }
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