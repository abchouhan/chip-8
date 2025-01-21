cmake_minimum_required(VERSION 3.25)
project(chip-8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SOURCE_FILES
	main.cpp
	src/Chip8.cpp
	src/Cpu.cpp
	src/Utils.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES} ${SDL2_LIBRARIES})
