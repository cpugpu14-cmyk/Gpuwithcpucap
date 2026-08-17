cmake_minimum_required(VERSION 3.15)
project(Gpuwithcpucap VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(ENABLE_GPU "Enable GPU support" OFF)
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_BENCHMARKS "Enable benchmarks" OFF)

# Add source directories
add_subdirectory(src)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Installation targets
install(
    DIRECTORY include/
    DESTINATION include
)
