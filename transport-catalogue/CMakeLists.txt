cmake_minimum_required(VERSION 3.23)
project(cpp-transport-catalogue)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp-transport-catalogue main.cpp domain.h domain.cpp geo.h geo.cpp json.h json.cpp json_reader.h json_reader.cpp map_renderer.h map_renderer.cpp
    request_handler.h request_handler.cpp svg.h svg.cpp transport_catalogue.h transport_catalogue.cpp json_builder.h json_builder.cpp)

set(CMAKE_CXX_FLAGS "-Wall -O3")
