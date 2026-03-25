cmake_minimum_required(VERSION 3.30)
project(Raycaster C)

set(CMAKE_C_STANDARD 17)

add_executable(Raycaster
        main.c
        libraries/level_loader.c
)

if (WIN32)

    message(STATUS "Building on Windows")

    set(SDL2_PATH "C:/SDL2")

    target_include_directories(Raycaster PRIVATE "${SDL2_PATH}/include")
    target_link_directories(Raycaster PRIVATE "${SDL2_PATH}/lib")

    target_link_libraries(Raycaster mingw32 SDL2main SDL2 SDL2_image)

elseif (UNIX)

    message(STATUS "Building on Linux / WSL")

    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    target_compile_options(Raycaster PRIVATE -g -O2 -fno-omit-frame-pointer)

    target_link_libraries(Raycaster
            SDL2::SDL2
            SDL2::SDL2main
            SDL2_image::SDL2_image
            SDL2_ttf::SDL2_ttf
            m
    )


endif()