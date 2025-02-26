#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define WINDOW_NAME "SIMPLE RAYCATER GAME"

#define MAP_WIDTH 9
#define MAP_HEIGHT 9

uint8_t map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 2, 0, 3, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 1, 0, 0, 0, 1, 4, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 2, 0, 3, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1}
};

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

    double playerX = 5.0;
    double playerY = 4.0;

    //Move vector
    double dirX = -1.0;
    double dirY = 0.0;
    //Camera
    double planeX =0;
    double planeY = 0.66; //Camera plane

    double time = 0.0;
    double oldTime = 0.0;

    double h = (double)SCREEN_HEIGHT;

    while (run) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
        }
        SDL_RenderClear(renderer);
        // Wypełnienie sufitu (górna połowa ekranu)
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Szary sufit
        SDL_Rect ceiling = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
        SDL_RenderFillRect(renderer, &ceiling);

        // Wypełnienie podłogi (dolna połowa ekranu)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Ciemniejsza podłoga
        SDL_Rect floor = {0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
        SDL_RenderFillRect(renderer, &floor);

        for (int x = 0; x < SCREEN_WIDTH; x++) {

            double cameraX = 2*x / (double)SCREEN_WIDTH -1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = (int)playerX;
            int mapY = (int)playerY;

            double sideDistX;
            double sideDistY;

            double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1.0 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1.0 / rayDirY);

            double perpWallDist;
            int stepX = 0;
            int stepY =0;
            int hit = 0;
            int side;
            if (rayDirX < 0) {
                stepX -= 1;
                sideDistX = (playerX - mapX) * deltaDistX;
            }
            else {
                stepX += 1;
                sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
            }
            if (rayDirY < 0) {
                stepY -= 1;
                sideDistY = (playerY - mapY) * deltaDistY;
            }
            else {
                stepY += 1;
                sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
            }
            while (hit==0) {
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                if (map[mapY][mapX] > 0) {
                    hit = 1;
                }
            }
            if (side == 0) {
                perpWallDist = sideDistX - deltaDistX;
            }else {
                perpWallDist = sideDistY - deltaDistY;
            }
            int lineHeight = (int)(h/perpWallDist);
            int drawStart = -lineHeight/2 + h/2;
            if (drawStart < 0) {
                drawStart = 0;
            }
            int drawEnd = lineHeight/2 + h/2;
            if (drawEnd >= h) {
                drawEnd = h-1;
            }
            SDL_Color color;
            switch (map[mapX][mapY]) {
                case 1:
                    color.r = 255;
                    color.g = 0;
                    color.b = 0;
                    break;
                default:
                    color.r = 255;
                    color.g = 255;
                    color.b = 0;
                    break;
            }
            if (side == 1) {
                color.r /=2;
                color.g /= 2;
                color.b /= 2;
            }
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
        }
        SDL_RenderPresent(renderer);

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(main_window);
    SDL_Quit();
    return 0;
}