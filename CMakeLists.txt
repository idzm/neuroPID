cmake_minimum_required(VERSION 3.13)

project(neuroPID)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")
add_executable(neuroPID WIN32 src/mlp.cpp src/nn_manager.cpp src/PID.cpp src/plant.cpp src/sample.cpp src/stdafx.cpp "src/main.cpp" src/neuroPID.rc)

target_include_directories(neuroPID PUBLIC src)
