cmake_minimum_required(VERSION 3.21)
project(lab5_buoyancy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(lab5_buoyancy main.cpp Physics.hpp)
target_link_libraries(lab5_buoyancy PRIVATE SFML::Graphics SFML::Window SFML::System)
