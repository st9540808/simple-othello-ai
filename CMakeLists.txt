cmake_minimum_required(VERSION 3.0.0)
project(othello VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB src "*.h" "*.cpp")
add_executable(othello ${src})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
