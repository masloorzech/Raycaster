# 🧱 Raycasting Rendering Engine

## 📖 Overview

This project is a simple real-time **raycasting-based rendering engine** written in C using SDL2.

It simulates a 3D environment using a 2D grid map and renders:

* walls (via DDA raycasting)
* floor and ceiling (floor casting)

The engine is designed with performance in mind and serves as a foundation for experimenting with low-level rendering optimizations.

---

## ⚙️ Features

* 🧱 Raycasted wall rendering (Wolfenstein-style)
* 🪵 Floor and ceiling rendering
* 🎮 Real-time player movement (keyboard + mouse)
* 🧵 Experimental multi-threading (WIP)
* 🖼️ Texture mapping
* ⚡ Optimized rendering pipeline (batched GPU upload)

---

## 🖥️ Screenshots

![Zrzut ekranu 2025-03-27 212008](https://github.com/user-attachments/assets/95f73518-1f5a-4a75-a4e3-1a3f13bb2545)
![raycaster](https://github.com/user-attachments/assets/40bcb487-9e58-47ca-9143-d8af07e2d3e5)
![Zrzut ekranu 2025-03-27 211950](https://github.com/user-attachments/assets/3bec7336-a75c-4e90-89b9-7e832bc7094b)

---

## 🏗️ Architecture

The engine uses a **software rendering pipeline**:

1. Scene is computed on CPU into a framebuffer (`buff`)
2. Walls are rendered using **DDA raycasting**
3. Floor and ceiling use **scanline-based floor casting**
4. Final image is uploaded to GPU using:

   ```
   SDL_UpdateTexture + SDL_RenderCopy
   ```

This replaces inefficient per-pixel rendering and significantly improves performance.

---

## ⚡ Performance

### 🔴 Previous approach

* Rendering each pixel using `SDL_RenderDrawPoint`
* ~500,000+ draw calls per frame
* High CPU overhead

### 🟢 Current approach

* Single framebuffer upload via `SDL_UpdateTexture`
* One draw call per frame
* Massive FPS improvement

---

## 🧵 Multithreading (WIP)

The engine includes an experimental multi-threaded implementation.

However, increasing the number of threads currently **does not improve performance**.

Possible reasons:

* Rendering bottleneck is not fully CPU-bound
* Workload is not evenly distributed
* Synchronization overhead reduces gains
* Parts of the pipeline remain single-threaded

This is an active area of investigation.

---

## 🧪 Build & Run

### Requirements

* C compiler (GCC / Clang / MSVC)
* SDL2
* SDL2_image
* CMake

---

### 🪟 Windows

```
cmake -B build
cmake --build build
```

---

### 🐧 Linux / WSL

```
sudo apt install libsdl2-dev libsdl2-image-dev

cmake -B build
cmake --build build
./build/Raycaster
```

---

## 📂 Map Format

```
width height
ceilingTexture floorTexture
playerStartX playerStartY

<map grid...>
```

Example:

```
12 12
3 3
2 2
1 1 1 ...
```

---

## 🧠 Learning Goals

This project explores:

* Low-level rendering techniques
* CPU vs GPU workload distribution
* Memory access patterns and cache locality
* Real-time optimization strategies
* Multithreading challenges in rendering engines

---

## 📌 To-Do

* Improve multithreading (proper workload splitting)
* Optimize floor casting
* Add dynamic lighting / shading
* Implement sprite rendering
* Add collision improvements

---

## 📄 License

MIT License

