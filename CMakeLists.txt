cmake_minimum_required(VERSION 3.16)
project(MatrixProject 
        VERSION 1.0.0 
        LANGUAGES CXX)

# Выбираем стандарт C++17 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS
    square_matrix.h
    diagonal_matrix.h
    benchmark.h
    dynamic_array.h
    exceptions.h
    complex.h
    slau_solver.h 
    iterator.h 
    option.h 
)


add_executable(matrix_app main.cpp ${HEADERS})
add_executable(matrix_tests test1.cpp ${HEADERS})


# Настройки для оптимизации Benchmark
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()