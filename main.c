#include <math.h>
#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_NAME "SIMPLE RAYCATER GAME"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

#define TEXTURES_NUMBER 64
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64



uint8_t map[MAP_HEIGHT][MAP_WIDTH] = {
    {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
  {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
  {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
  {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
  {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
  {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
  {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
  {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

void swap(uint32_t* a, uint32_t* b) {
    uint32_t temp = *a;
    *a = *b;
    *b = temp;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING)!=0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);

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

    double playerAngle = -M_PI / 2;  // Początkowy kąt - patrzymy na lewo (90° w lewo od osi X)

    Uint32 (*buff)[SCREEN_WIDTH] = malloc(SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
    Uint32 (*textures)[TEXTURE_WIDTH * TEXTURE_HEIGHT] = malloc(TEXTURES_NUMBER * TEXTURE_WIDTH * TEXTURE_HEIGHT * sizeof(Uint32));

    if (!buff || !textures) {
        printf("Memory allocation failed\n");
        return -1; // Handle allocation failure
    }
    for (int x = 0; x < TEXTURE_WIDTH; x++) {
        for (int y = 0; y < TEXTURE_HEIGHT; y++) {
            int xorcolor = (x * 256 / TEXTURE_WIDTH) ^ (y * 256 / TEXTURE_HEIGHT);
            int ycolor = y * 256 / TEXTURE_HEIGHT;
            int xycolor = y * 128 / TEXTURE_HEIGHT + x * 128 / TEXTURE_WIDTH;

            textures[0][TEXTURE_WIDTH * y + x] = 65536 * 254 * (x != y && x != TEXTURE_WIDTH - y); // Flat red texture with black cross
            textures[1][TEXTURE_WIDTH * y + x] = xycolor + 256 * xycolor + 65536 * xycolor; // Sloped greyscale
            textures[2][TEXTURE_WIDTH * y + x] = 256 * xycolor + 65536 * xycolor; // Sloped yellow gradient
            textures[3][TEXTURE_WIDTH * y + x] = xorcolor + 256 * xorcolor + 65536 * xorcolor; // XOR greyscale
            textures[4][TEXTURE_WIDTH * y + x] = 256 * xorcolor; // XOR green
            textures[5][TEXTURE_WIDTH * y + x] = 65536 * 192 * (x % 16 && y % 16); // Red bricks
            textures[6][TEXTURE_WIDTH * y + x] = 65536 * ycolor; // Red gradient
            textures[7][TEXTURE_WIDTH * y + x] = 128 + 256 * 128 + 65536 * 128; // Flat grey texture
        }
    }

    bool moveForward = false, moveBackward = false;
    bool moveLeft = false, moveRight = false;
    while (run) {


        SDL_RenderClear(renderer);
        // Wypełnienie sufitu (górna połowa ekranu)
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Szary sufit
        SDL_Rect ceiling_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
        SDL_RenderFillRect(renderer, &ceiling_rect);

        // Wypełnienie podłogi (dolna połowa ekranu)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Ciemniejsza podłoga
        SDL_Rect floor_rect = {0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
        SDL_RenderFillRect(renderer, &floor_rect);

        for (int x = 0; x < SCREEN_WIDTH; x++) {

            double cameraX = 2*x / (double)SCREEN_WIDTH -1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = (int)playerX;
            int mapY = (int)playerY;

            double sideDistX =0;
            double sideDistY =0;

            double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);

            double perpWallDist=0;
            int stepX = 0;
            int stepY =0;
            int hit =0;
            int side=0;

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
                if (map[mapX][mapY] > 0) {
                    hit = 1;
                }
            }

            if (side == 0) {
                perpWallDist = sideDistX - deltaDistX;
            }else {
                perpWallDist = sideDistY - deltaDistY;
            }
            int lineHeight = (int)(h/perpWallDist);
            int drawStart = (int)(-lineHeight/2.0 + h/2.0);
            if (drawStart < 0) {
                drawStart = 0;
            }
            int drawEnd = (int)(lineHeight/2.0 + h/2.0);
            if (drawEnd >= h) {
                drawEnd = (int)(h-1.0);
            }

            int texNum = map[mapX][mapY]-1;

            double wallX =0;
            if (side == 0) {
                wallX = playerY + perpWallDist * rayDirY;
            }else {
                wallX = playerX + perpWallDist * rayDirX;
            }

            wallX -= floor(wallX);

            int texX = (int)(wallX * (double)TEXTURE_WIDTH);
            if (side == 0 && rayDirX > 0) {
                texX = TEXTURE_WIDTH - texX-1;
            }
            if (side == 1 && rayDirY < 0) {
                texX = TEXTURE_WIDTH - texX-1;
            }

            double step = 1.0 * TEXTURE_WIDTH / lineHeight;

            double texPos = (drawStart - h/2.0 + lineHeight/2.0) * step;
            for (int y = drawStart; y < drawEnd; y++) {
                int texY = (int)texPos & (TEXTURE_HEIGHT - 1);
                texPos+=step;
                Uint32 color = textures[texNum][TEXTURE_HEIGHT * texY + texX];
                if (side ==1) {
                    color = (color >> 1) & 8355711;
                }
                buff[y][x] = color;
            }

            for (int y = drawStart; y < drawEnd; y++) {
                uint32_t color = buff[y][x];
                SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);

        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                buff[i][j] = 0;
            }
        }

        oldTime = time;
        time = SDL_GetTicks();
        double frameTime = (time - oldTime) / 1000.0;

        double moveSpeed = frameTime * 5.0;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) run = false;
                if (event.key.keysym.sym == SDLK_w) moveForward = true;
                if (event.key.keysym.sym == SDLK_s) moveBackward = true;
                if (event.key.keysym.sym == SDLK_a) moveLeft = true;
                if (event.key.keysym.sym == SDLK_d) moveRight = true;
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_w) moveForward = false;
                if (event.key.keysym.sym == SDLK_s) moveBackward = false;
                if (event.key.keysym.sym == SDLK_a) moveLeft = false;
                if (event.key.keysym.sym == SDLK_d) moveRight = false;
            }

            int mouseX, mouseY;
            SDL_GetRelativeMouseState(&mouseX, &mouseY);

            // Zmienna kontrolująca czułość obrotu
            double mouseSensitivity = 0.002;  // Zmieniaj tę wartość, aby dostosować czułość obrotu

            // Prędkość rotacji na podstawie zmiany pozycji myszy w poziomie
            playerAngle += mouseX * mouseSensitivity;

            // Zabezpieczenie przed przekroczeniem granic
            if (playerAngle < -M_PI) playerAngle += 2 * M_PI;
            if (playerAngle > M_PI) playerAngle -= 2 * M_PI;

            // Zaktualizuj kierunek
            dirX = cos(playerAngle);
            dirY = sin(playerAngle);

            // Zaktualizowanie kamery (plane)
            planeX = cos(playerAngle + M_PI / 2) * 0.66;
            planeY = sin(playerAngle + M_PI / 2) * 0.66;
        }

        if (moveForward) {
            if (map[(int)(playerX + dirX * moveSpeed)][(int)playerY] == 0) {
                playerX += dirX * moveSpeed;
            }

            if (map[(int)playerX][(int)(playerY + dirY * moveSpeed)] == 0) {
                playerY += dirY * moveSpeed;
            }

        }
        if (moveBackward) {
            if (map[(int)(playerX - dirX * moveSpeed)][(int)playerY] == 0) {
                playerX -= dirX * moveSpeed;
            }
            if (map[(int)playerX][(int)(playerY - dirY * moveSpeed)] == 0) {
                playerY -= dirY * moveSpeed;
            }
        }
        if (moveLeft) {
            if (map[(int)(playerX - planeX * moveSpeed)][(int)playerY] == 0) {
                playerX -= planeX * moveSpeed;
            }
            if (map[(int)playerX][(int)(playerY - planeY * moveSpeed)] == 0) {
                playerY -= planeY * moveSpeed;
            }
        }

        if (moveRight) {
            if (map[(int)(playerX + planeX * moveSpeed)][(int)playerY] == 0) {
                playerX += planeX * moveSpeed;
            }
            if (map[(int)playerX][(int)(playerY + planeY * moveSpeed)] == 0) {
                playerY += planeY * moveSpeed;
            }
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(main_window);
    SDL_Quit();
    free(buff);
    free(textures);
    return 0;
}