# CMakeLists.txt

cmake_minimum_required(VERSION 3.14)

# Project name and version
project(myjoin VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Directories
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)

# Add the main executable
add_executable(myjoin
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/joiner.cpp
    ${SRC_DIR}/utils.cpp
)

# Include directories for the main executable
target_include_directories(myjoin PRIVATE ${INCLUDE_DIR})

# Enable warnings
if (MSVC)
    target_compile_options(myjoin PRIVATE /W4)
else()
    target_compile_options(myjoin PRIVATE -Wall -Wextra -pedantic)
endif()

# Add the test executable
add_executable(myjoin_tests
    ${TEST_DIR}/test_join.cpp
    ${TEST_DIR}/test_utils.cpp
)

# Include directories for the test executable
target_include_directories(myjoin_tests PRIVATE ${INCLUDE_DIR})

# Link the same libraries as the main executable if needed
target_link_libraries(myjoin_tests PRIVATE)

# Add a custom target to run tests
add_custom_target(run_tests
    COMMAND myjoin_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running unit tests..."
)
