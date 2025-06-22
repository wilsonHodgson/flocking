cmake_minimum_required(VERSION 3.11)
project(Duck_Flock)
#add_subdirectory(flocking)

# This variable exports a json for the C language server to interpret
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# Locate libraries
set(FLOCKING_SYSTEM flocking/ducks.c)

include_directories(lib)

link_libraries(raylib m)
add_executable(duck_flock main.c ${FLOCKING_SYSTEM}) 
