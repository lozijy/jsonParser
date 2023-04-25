cmake_minimum_required(VERSION 3.25)
project(jsonParser)

set(CMAKE_CXX_STANDARD 14)

add_executable(jsonParser
        jsonParser.cpp test.cpp)
