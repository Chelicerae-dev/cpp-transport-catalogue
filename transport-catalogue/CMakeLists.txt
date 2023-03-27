cmake_minimum_required(VERSION 3.23)
project(cpp-transport-catalogue)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp-transport-catalogue main.cpp input_reader.h input_reader.cpp stat_reader.h stat_reader.cpp transport_catalogue.h transport_catalogue.cpp geo.h)

set(CMAKE_CXX_FLAGS "-Wall -O3")
