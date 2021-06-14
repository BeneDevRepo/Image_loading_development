cmake_minimum_required(VERSION 3.0.0)
project(Software_Rasterizer VERSION 0.1.0)

include(CTest)
enable_testing()

add_compile_options(-fconcepts -Wall -Wpedantic)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(Software_Rasterizer ${SRC})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)