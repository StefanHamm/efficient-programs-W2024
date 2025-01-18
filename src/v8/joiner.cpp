#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <x86intrin.h>

struct Table1Entry {
    std::vector<std::string> Bs;
    std::vector<std::string> Cs;
};


// Helper function to get file size
size_t getFileSize(int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) {
        return 0;
    }
    return st.st_size;
}

// Helper function to map file into memory
char* mapFile(const std::string& path, size_t& fileSize) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        return nullptr;
    }
    fileSize = getFileSize(fd);
    if (fileSize == 0) {
        close(fd);
        return nullptr;
    }
    char* addr = (char*)mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd); // File descriptor is no longer needed after mmap
    if (addr == MAP_FAILED) {
        return nullptr;
    }
    return addr;
}

void unmapFile(char* addr, size_t fileSize) {
    if (addr != nullptr) {
        munmap(addr, fileSize);
    }
}


// New parseLine function that operates on memory mapped data
void parseLineMMAP(const char* data, size_t& pos, size_t fileSize, char delimiter, std::string& part1, std::string& part2) {
    size_t start = pos;
    while (pos < fileSize && data[pos] != delimiter && data[pos] != '\n') {
        pos++;
    }
    if (pos < fileSize) {
        part1.assign(data + start, pos - start);
        if (data[pos] == delimiter)
        {
            start = ++pos;
            while (pos < fileSize && data[pos] != '\n') {
                pos++;
            }
            part2.assign(data + start, pos - start);
        }
        
        if (pos < fileSize && data[pos] == '\n') pos++;
    }
}

int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1;
    table1.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table3;
    table3.reserve(20000000);

    std::unordered_map<std::string, std::vector<std::string>> table4;
    table4.reserve(20000000);

    // Memory-map the files
    size_t fileSize1, fileSize2, fileSize3, fileSize4;
    char* file1_data = mapFile(path1, fileSize1);
    char* file2_data = mapFile(path2, fileSize2);
    char* file3_data = mapFile(path3, fileSize3);
    char* file4_data = mapFile(path4, fileSize4);

    if (!file1_data || !file2_data || !file3_data || !file4_data) {
        std::cerr << "Error mapping files\n";
        unmapFile(file1_data, fileSize1);
        unmapFile(file2_data, fileSize2);
        unmapFile(file3_data, fileSize3);
        unmapFile(file4_data, fileSize4);
        return -1;
    }

    // Read File1 (A,B)
    size_t pos = 0;
    std::string A, B;
    while (pos < fileSize1) {
       parseLineMMAP(file1_data, pos, fileSize1, ',', A, B);
       if (!A.empty()) table1[A].Bs.push_back(B);
    }


    // Read File2 (A,C)
    pos = 0;
    std::string C;
    while (pos < fileSize2) {
       parseLineMMAP(file2_data, pos, fileSize2, ',', A, C);
       if (!A.empty()) table1[A].Cs.push_back(C);
    }


    // Read File3 (A,D)
    pos = 0;
    std::string D;
    while (pos < fileSize3) {
       parseLineMMAP(file3_data, pos, fileSize3, ',', A, D);
       if (!A.empty()) table3[A].push_back(D);
    }


    // Read File4 (D,E)
    pos = 0;
    std::string E;
    while (pos < fileSize4) {
       parseLineMMAP(file4_data, pos, fileSize4, ',', D, E);
       if (!D.empty()) table4[D].push_back(E);
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

    // Unmap memory
    unmapFile(file1_data, fileSize1);
    unmapFile(file2_data, fileSize2);
    unmapFile(file3_data, fileSize3);
    unmapFile(file4_data, fileSize4);
    return 0;
}

int join(const std::string path1, const std::string path2, const std::string path3, const std::string path4) {
    int code = hashJoin(path1, path2, path3, path4);
    return code;
}