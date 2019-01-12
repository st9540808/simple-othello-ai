cmake_minimum_required(VERSION 3.0.0)
project(othello VERSION 0.1.0)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -fopenmp -pthread -std=gnu++17")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_COMPILER "g++")

if (CMAKE_BUILD_TYPE MATCHES RELEASE)
    message("Add NDEBUG Macro")
    add_definitions(-DNDEBUG)
endif()

include(CTest)
enable_testing()

file(GLOB src "*.h" "*.cpp")
add_executable(othello ${src})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
