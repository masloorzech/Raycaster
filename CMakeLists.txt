cmake_minimum_required(VERSION 3.30)
project(Raycaster C)

set(CMAKE_C_STANDARD 17)

# Ścieżka do SDL
set(SDL2_PATH "C:/SDL2")

# Dodajemy SDL do kompilacji
include_directories(${SDL2_PATH}/include)
link_directories(${SDL2_PATH}/lib)

find_package(Threads REQUIRED)

add_executable(Raycaster main.c
        libraries/level_loader.c
        libraries/level_loader.h)

target_link_libraries(Raycaster mingw32 SDL2main SDL2 SDL2_image Threads::Threads)
