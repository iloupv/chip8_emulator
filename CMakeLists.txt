cmake_minimum_required(VERSION 3.14)
project(chip8)

set(CMAKE_CXX_STANDARD 11)

find_package(SDL2 REQUIRED)
include_directories(${SDL_INCLUDE_DIRS})

add_executable(
	chip8
	src/Chip8.cpp
	src/Main.cpp
	src/PlatformDisplay.cpp
	src/PlatformSound.cpp
	src/PlatformInput.cpp)

target_compile_options(chip8 PRIVATE -Wall)

target_link_libraries(chip8 ${SDL2_LIBRARIES})
