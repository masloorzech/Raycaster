#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define WINDOW_NAME "SIMPLE RAYCATER GAME"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING)!=0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Window* main_window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (main_window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer * renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Event event;
    bool run = true;
    while (run) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(main_window);
    SDL_Quit();
    return 0;
}