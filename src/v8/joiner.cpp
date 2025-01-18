#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <x86intrin.h>

// Custom struct to avoid string allocations for values
struct Value {
    const char* data;
    size_t size;
};


struct Table1Entry {
    std::vector<Value> Bs;
    std::vector<Value> Cs;
};

// Helper function to get file size (same as before)
size_t getFileSize(int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) {
        return 0;
    }
    return st.st_size;
}

// Helper function to map file into memory (same as before)
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
    close(fd);
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

// Optimized parsing function that returns pointers and sizes
inline void parseLineMMAP(const char* data, size_t& pos, size_t fileSize, char delimiter, Value& part1, Value& part2) {
    size_t start = pos;
     while (pos < fileSize && data[pos] != delimiter && data[pos] != '\n') {
        pos++;
    }
    if (pos < fileSize) {
        part1.data = data + start;
        part1.size = pos - start;
        if (data[pos] == delimiter)
        {
            start = ++pos;
            while (pos < fileSize && data[pos] != '\n') {
                pos++;
            }
            part2.data = data + start;
            part2.size = pos - start;
        } else {
           part2.data = nullptr;
           part2.size = 0;
        }
        if (pos < fileSize && data[pos] == '\n') pos++;
    } else {
        part1.data = nullptr;
        part1.size = 0;
        part2.data = nullptr;
        part2.size = 0;
    }
}


// String equality using memcmp
inline bool value_equal(const Value& a, const Value& b) {
    return (a.size == b.size) && (std::memcmp(a.data, b.data, a.size) == 0);
}


// String Hash using FNV-1a
inline size_t value_hash(const Value& v) {
   size_t hash = 2166136261U;
    for (size_t i = 0; i < v.size; ++i) {
        hash ^= (unsigned char)(v.data[i]);
        hash *= 16777619U;
    }
    return hash;
}

// Custom Hash for std::string_view based on value_hash
struct ValueHash {
    size_t operator()(const Value& v) const {
        return value_hash(v);
    }
};


// Custom Equal to use value_equal with Value
struct ValueEqual {
    bool operator()(const Value& a, const Value& b) const {
        return value_equal(a,b);
    }
};



// Hash table with Value as key
using HashTableValues = std::unordered_map<Value, Table1Entry, ValueHash, ValueEqual>;
using HashTableValueToValues = std::unordered_map<Value, std::vector<Value>, ValueHash, ValueEqual>;


int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    HashTableValues table1;
    table1.reserve(20000000);

    HashTableValueToValues table3;
    table3.reserve(20000000);

    HashTableValueToValues table4;
    table4.reserve(20000000);

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
    Value A, B;
    while (pos < fileSize1) {
        parseLineMMAP(file1_data, pos, fileSize1, ',', A, B);
        if (A.data != nullptr) table1[A].Bs.push_back(B);
    }
    

    // Read File2 (A,C)
    pos = 0;
    Value C;
    while (pos < fileSize2) {
        parseLineMMAP(file2_data, pos, fileSize2, ',', A, C);
        if (A.data != nullptr) table1[A].Cs.push_back(C);
    }

    // Read File3 (A,D)
     pos = 0;
    Value D;
    while (pos < fileSize3) {
       parseLineMMAP(file3_data, pos, fileSize3, ',', A, D);
        if(A.data != nullptr) table3[A].push_back(D);
    }
    

    // Read File4 (D,E)
    pos = 0;
    Value E;
    while (pos < fileSize4) {
       parseLineMMAP(file4_data, pos, fileSize4, ',', D, E);
       if (D.data != nullptr) table4[D].push_back(E);
    }


    // Perform the join
    for (const auto& [A, entry] : table1) {
        if (table3.find(A) != table3.end()) {
            for (const auto& D : table3[A]) {
                if (table4.find(D) != table4.end()) {
                    for (const auto& E : table4[D]) {
                        for (const auto& B : entry.Bs) {
                            for (const auto& C : entry.Cs) {
                                std::cout.write(D.data, D.size);
                                std::cout << ",";
                                std::cout.write(A.data, A.size);
                                std::cout << ",";
                                std::cout.write(B.data, B.size);
                                std::cout << ",";
                                std::cout.write(C.data, C.size);
                                std::cout << ",";
                                std::cout.write(E.data, E.size);
                                std::cout << std::endl;
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