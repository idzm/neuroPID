cmake_minimum_required(VERSION 3.13)

project(neuroPID)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")

add_executable(neuroPID WIN32 
	src/mlp.cpp 
	src/nn_manager.cpp 
	src/PID.cpp 
	src/plant.cpp 
	src/sample.cpp 
	 
	src/main.cpp 
	src/neuroPID.rc)

add_executable(emulator_learn
	src/mlp.cpp 
	src/nn_manager.cpp 
	src/PID.cpp 
	src/plant.cpp 
	src/sample.cpp 
	 
	src/emulator_static_learn/main.cpp)

add_custom_command(
        TARGET neuroPID emulator_learn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
                ${CMAKE_SOURCE_DIR}/src/data
                ${CMAKE_CURRENT_BINARY_DIR}/data)

target_include_directories(neuroPID PUBLIC src)
target_include_directories(emulator_learn PUBLIC src)
