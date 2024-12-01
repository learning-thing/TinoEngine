cmake_minimum_required(VERSION 3.29.2)
project(default)

#include_directories( lib/ )
include_directories( src/lib/ )

add_executable( main src/main.cpp )

#add_executable( cullingTest src/Culling.cpp)

target_link_libraries(main SDL3 SDL3_image)
#target_link_libraries(cullingTest SDL2 SDL2_image)