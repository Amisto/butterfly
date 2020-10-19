cmake_minimum_required(VERSION 3.16)
project(Solver)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB SOURCE_FILES "src/*.cpp")
add_executable(Solver ${SOURCE_FILES})