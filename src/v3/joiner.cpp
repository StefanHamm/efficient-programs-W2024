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

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1; // Key: A, Values: Bs and Cs
    table1.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table3; // Key: A, Values: D's
    table3.reserve(15000000);

    std::unordered_map<std::string, std::vector<std::string>> table4; // Key: D, Values: E's
    table4.reserve(15000000);


    
    // Read File1 (A,B)
    std::ifstream file1(path1);
    if (!file1.is_open()) {
        std::cerr << "Error opening File1\n";
        return -1;
    }
    std::string line, A, B;
    while (std::getline(file1, line)) {
        size_t delimPos = line.find(',');
        if (delimPos != std::string::npos) {
            A = line.substr(0, delimPos);
            B = line.substr(delimPos + 1);
            //if (table1[A].Bs.empty()) {
            //    table1[A].Bs.reserve(1000000);
            //}
            table1[A].Bs.push_back(B);
        }
        
    }

    // Read File2 (A,C)
    std::ifstream file2(path2);
    if (!file2.is_open()) {
        std::cerr << "Error opening File2\n";
        return -1;
    }
    std::string C;
    while (std::getline(file2, line)) {
        size_t delimPos = line.find(',');
        if (delimPos != std::string::npos) {
            A = line.substr(0, delimPos);
            C = line.substr(delimPos + 1);
            //if (table1[A].Cs.empty()) {
            //    table1[A].Cs.reserve(1000000); // Initial reserve for small Cs
            //}
            table1[A].Cs.push_back(C);

        }
    }

    // Read File3 (A,D)
    std::ifstream file3(path3);
    if (!file3.is_open()) {
        std::cerr << "Error opening File3\n";
        return -1;
    }
    std::string D;
    while (std::getline(file3, line)) {
        size_t delimPos = line.find(',');
        if (delimPos != std::string::npos) {
            A = line.substr(0, delimPos);
            D = line.substr(delimPos + 1);
            //if (table3[A].empty()) {
            //    table3[A].reserve(1000000); // Initial reserve for small Cs
            //}
            table3[A].push_back(D);
        }
    }

    // Read File4 (D,E)
    std::ifstream file4(path4);
    if (!file4.is_open()) {
        std::cerr << "Error opening File4\n";
        return -1;
    }
    std::string E;
    while (std::getline(file4, line)) {
        size_t delimPos = line.find(',');
        if (delimPos != std::string::npos) {
            D = line.substr(0, delimPos);
            E = line.substr(delimPos + 1);

            //if (table4[D].empty()) {
            //    table4[D].reserve(1000000); // Initial reserve for small Es
            //}
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