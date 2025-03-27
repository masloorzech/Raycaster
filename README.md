# Wall Rendering Engine

## Overview
This project is a simple 3D rendering engine designed to render walls in a scene. It uses multi-threading to optimize rendering performance, but currently, adding more threads does not seem to improve render time.

## Features
- Renders walls efficiently in a 3D space
- Uses multi-threading for parallel processing
- Supports basic optimizations for performance

## Screenshots

![Zrzut ekranu 2025-03-27 212008](https://github.com/user-attachments/assets/95f73518-1f5a-4a75-a4e3-1a3f13bb2545)
![raycaster](https://github.com/user-attachments/assets/40bcb487-9e58-47ca-9143-d8af07e2d3e5)
![Zrzut ekranu 2025-03-27 211950](https://github.com/user-attachments/assets/3bec7336-a75c-4e90-89b9-7e832bc7094b)

## Requirements
- C++ compiler (GCC/Clang/MSVC)
- SDL2
- CMake (for building)
- A system supporting multi-threading

## Performance Issue
Despite adding 4 threads, the rendering time does not improve. Possible reasons:
- The rendering task is not CPU-bound
- Synchronization overhead negates benefits
- Some parts of the rendering process remain single-threaded

## To-Do
- Investigate multi-threading efficiency
- Optimize wall rendering
- Add more advanced rendering features

## License
This project is open-source and distributed under the MIT License.

