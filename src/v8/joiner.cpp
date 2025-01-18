#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <immintrin.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "joiner.h"

// Chunk size (adjust based on your system's memory and performance characteristics)
const size_t CHUNK_SIZE = 1024 * 1024 * 100; // 100 MB
const int VECTOR_SIZE = 16; // SIMD vector size

struct Table1Entry {
    std::vector<std::string> Bs;
    std::vector<std::string> Cs;
};

// Optimized parsing function using SIMD to find delimiters and split lines
void parseLine(const char* line, size_t lineSize, char delimiter, std::string& part1, std::string& part2) {
     size_t delimPos = std::string::npos;
    __m128i delimiterVec = _mm_set1_epi8(delimiter);
    size_t i = 0;
    for (; i + VECTOR_SIZE <= lineSize; i += VECTOR_SIZE) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(line + i));
        __m128i mask = _mm_cmpeq_epi8(chunk, delimiterVec);
        unsigned int maskValue = _mm_movemask_epi8(mask);
        if (maskValue != 0) {
            delimPos = i + __builtin_ctz(maskValue);
            break;
        }
    }
    if (delimPos == std::string::npos) {
        for (; i < lineSize; ++i) {
            if (line[i] == delimiter) {
                delimPos = i;
                break;
            }
        }
    }
    if (delimPos != std::string::npos) {
        part1 = std::string(line, 0, delimPos);
        part2 = std::string(line, delimPos + 1, lineSize - (delimPos+1));
    }
}


// Function to process a chunk of a memory mapped file (generic)
template <typename TableType>
void processChunk(const char* fileStart, size_t fileSize, char delimiter, TableType& table) {
    const char* current = fileStart;
    const char* end = fileStart + fileSize;
     while (current < end) {
          const char* lineStart = current;
        
          while (current < end && *current != '\n') {
             ++current;
          }
          size_t lineSize = current - lineStart;
        
           if (lineSize > 0) {
                std::string line, key, value;
                parseLine(lineStart, lineSize, delimiter, key, value);
                table[key].push_back(value);
            }
          ++current;
     }
}

// Function to process a chunk of file1 and file2 (specific)
void processChunk(const char* file1Start, size_t file1Size, const char* file2Start, size_t file2Size, char delimiter, std::unordered_map<std::string, Table1Entry>& table1, bool isFirstChunk) {
    const char* current1 = file1Start;
    const char* end1 = file1Start + file1Size;

     const char* current2 = file2Start;
     const char* end2 = file2Start + file2Size;


    if (isFirstChunk) {
         while(current1 < end1) {
             const char* lineStart = current1;
             while(current1 < end1 && *current1 != '\n') ++current1;
              size_t lineSize = current1 - lineStart;

              if (lineSize > 0) {
                   std::string line, A, B;
                  parseLine(lineStart, lineSize, delimiter, A, B);
                  table1[A].Bs.push_back(B);
              }
            ++current1;
        }

      while(current2 < end2) {
           const char* lineStart = current2;
             while(current2 < end2 && *current2 != '\n') ++current2;
             size_t lineSize = current2 - lineStart;

           if(lineSize > 0) {
             std::string line, A, C;
              parseLine(lineStart, lineSize, delimiter, A, C);
              table1[A].Cs.push_back(C);
         }
            ++current2;
      }
    } else {
         // Re-process to update table1. Note that memory mapped files dont need to be "reset", and we simply iterate from the beginning of the file
       
          while(current1 < end1) {
             const char* lineStart = current1;
             while(current1 < end1 && *current1 != '\n') ++current1;
             size_t lineSize = current1 - lineStart;

           if (lineSize > 0) {
                  std::string line, A, B;
                   parseLine(lineStart, lineSize, delimiter, A, B);
                    table1[A].Bs.push_back(B);
              }
              ++current1;
        }

      while(current2 < end2) {
           const char* lineStart = current2;
             while(current2 < end2 && *current2 != '\n') ++current2;
            size_t lineSize = current2 - lineStart;

          if(lineSize > 0) {
               std::string line, A, C;
                 parseLine(lineStart, lineSize, delimiter, A, C);
                  table1[A].Cs.push_back(C);
             }
            ++current2;
        }
    }
}


int hashJoin(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4) {
    std::unordered_map<std::string, Table1Entry> table1;
    std::unordered_map<std::string, std::vector<std::string>> table3;
    std::unordered_map<std::string, std::vector<std::string>> table4;

    // Open all files
    int fd1 = open(path1.c_str(), O_RDONLY);
    int fd2 = open(path2.c_str(), O_RDONLY);
    int fd3 = open(path3.c_str(), O_RDONLY);
    int fd4 = open(path4.c_str(), O_RDONLY);
    if (fd1 == -1 || fd2 == -1 || fd3 == -1 || fd4 == -1) {
      std::cerr << "Error opening files\n";
        return -1;
    }

    // Get file sizes
    struct stat st1, st2, st3, st4;
    fstat(fd1, &st1);
    fstat(fd2, &st2);
    fstat(fd3, &st3);
    fstat(fd4, &st4);
    size_t size1 = st1.st_size;
    size_t size2 = st2.st_size;
    size_t size3 = st3.st_size;
    size_t size4 = st4.st_size;


    // Map the files into memory
    const char* file1_mmap = static_cast<const char*>(mmap(nullptr, size1, PROT_READ, MAP_PRIVATE, fd1, 0));
    const char* file2_mmap = static_cast<const char*>(mmap(nullptr, size2, PROT_READ, MAP_PRIVATE, fd2, 0));
    const char* file3_mmap = static_cast<const char*>(mmap(nullptr, size3, PROT_READ, MAP_PRIVATE, fd3, 0));
    const char* file4_mmap = static_cast<const char*>(mmap(nullptr, size4, PROT_READ, MAP_PRIVATE, fd4, 0));
     if (file1_mmap == MAP_FAILED || file2_mmap == MAP_FAILED || file3_mmap == MAP_FAILED || file4_mmap == MAP_FAILED) {
        std::cerr << "Error memory mapping files\n";
         close(fd1);
         close(fd2);
         close(fd3);
         close(fd4);
        return -1;
    }

    // Process file1 and file2
    bool isFirstChunk = true;
     processChunk(file1_mmap, size1, file2_mmap, size2, ',', table1, isFirstChunk);


    // Process file3
    processChunk(file3_mmap, size3, ',', table3);

    // Process file4
     processChunk(file4_mmap, size4, ',', table4);

    // Unmap the files
    munmap( (void *)file1_mmap, size1);
    munmap( (void *)file2_mmap, size2);
    munmap( (void *)file3_mmap, size3);
    munmap( (void *)file4_mmap, size4);


    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);


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