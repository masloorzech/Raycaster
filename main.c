#include <math.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>


#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 480
#define WINDOW_NAME "SIMPLE RAYCATER GAME"

#define MAP_WIDTH 12
#define MAP_HEIGHT 12

#define TEXTURES_NUMBER 11
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/barrel.png", //0
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/bluestone.png", //1
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/colorstone.png", //2
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/eagle.png", //3
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/greenlight.png", //4
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/greystone.png", //5
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/mossy.png", //6
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/pillar.png", //7
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/purplestone.png", //8
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/redbrick.png", //9
//         "C:/Users/anton/CLionProjects/Raycaster/assets/textures/wood.png" //10

uint8_t map[MAP_HEIGHT][MAP_WIDTH] = {
{5,5,5,5,5,5,5,5,5,5,5,5},
{5,0,10,0,0,0,0,0,0,0,0,5},
{5,0,10,10,10,10,10,0,0,0,0,5},
{5,0,10,0,0,0,0,0,0,0,0,5},
{6,0,10,0,0,0,0,0,0,0,0,5},
{6,0,0,0,0,0,5,5,5,5,5,5},
{6,0,0,0,0,0,0,0,0,0,0,5},
{6,0,0,0,0,0,9,9,9,9,9,9},
{5,0,0,0,0,0,9,0,0,9,9,9},
{5,0,0,0,0,0,0,0,0,3,9,9},
{5,0,0,0,0,0,9,0,0,9,9,9},
{5,5,5,5,5,5,9,9,9,9,9,9}
};


int load_texture(const char* file_path, Uint32* texture) {
    SDL_Surface* surface = IMG_Load(file_path);
    if (!surface) {
        printf("Nie udało się wczytać obrazka: %s\n", IMG_GetError());
        return -1;
    }

    if (surface->w != TEXTURE_WIDTH || surface->h != TEXTURE_HEIGHT) {
        printf("Obrazek ma inne wymiary niż oczekiwane!\n");
        SDL_FreeSurface(surface);
        return -1;
    }

    SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (!converted_surface) {
        printf("Nie udało się przekonwertować powierzchni: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return -1;
    }
    Uint32* pixels = (Uint32*)converted_surface->pixels;
    for (int i = 0; i < TEXTURE_WIDTH * TEXTURE_HEIGHT; i++) {
        texture[i] = pixels[i];
    }
    SDL_FreeSurface(converted_surface);
    SDL_FreeSurface(surface);
    return 0;
}


int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING)!=0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return -1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

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

    Uint32** buff = (Uint32**)malloc(SCREEN_HEIGHT * sizeof(Uint32*));
    if (!buff) {
        printf("Błąd alokacji pamięci!\n");
        return -1;
    }

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        buff[i] = (Uint32*)malloc(SCREEN_WIDTH * sizeof(Uint32));
        if (!buff[i]) {
            printf("Błąd alokacji pamięci dla wiersza %d!\n", i);
            for (int j = 0; j < i; j++) {
                free(buff[j]);
            }
            free(buff);
            return -1;
        }
    }

    Uint32** textures;

    textures = (Uint32**)malloc(TEXTURES_NUMBER * sizeof(Uint32*));
    if (textures == NULL) {
        printf("Błąd alokacji pamięci!\n");
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            free(buff[i]);
        }
        free(buff);
        return -1;
    }

    for (int i = 0; i < TEXTURES_NUMBER; i++) {
        textures[i] = (Uint32*)calloc(TEXTURE_WIDTH * TEXTURE_HEIGHT ,sizeof(Uint32));
        if (textures[i] == NULL) {
            printf("Błąd alokacji pamięci dla tekstury %d!\n", i);
            for (int k = 0; k < SCREEN_HEIGHT; k++) {
                free(buff[i]);
            }
            free(buff);
            return -1;
        }
    }

    const char* file_paths[TEXTURES_NUMBER] = {
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/barrel.png", //0
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/bluestone.png", //1
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/colorstone.png", //2
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/eagle.png", //3
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/greenlight.png", //4
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/greystone.png", //5
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/mossy.png", //6
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/pillar.png", //7
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/purplestone.png", //8
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/redbrick.png", //9
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/wood.png" //10
    };

    for (int i = 0; i < TEXTURES_NUMBER; i++) {
        if (load_texture(file_paths[i], textures[i]) == -1) {
            printf("Nie udało się wczytać obrazka: %s\n", file_paths[i]);
            for (int j = 0; j <= i; j++) {
                free(textures[j]);
            }
            free(textures);
            IMG_Quit();
            SDL_Quit();
            return -1;
        }
    }

    bool moveForward = false, moveBackward = false;
    bool moveLeft = false, moveRight = false;

    printf("Loaded all files correctly!!!");

    SDL_Window* main_window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (main_window == NULL) {
        for (int i = 0; i < TEXTURES_NUMBER; i++) {
            free(textures[i]);
        }
        free(textures);
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            free(buff[i]);
        }
        free(buff);
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer * renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        for (int i = 0; i < TEXTURES_NUMBER; i++) {
            free(textures[i]);
        }
        free(textures);
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            free(buff[i]);
        }
        free(buff);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return -1;
    }

    while (run) {

        for (int y = 0; y < h; y++) {

            double rayDirX0 = dirX - planeX;
            double rayDirY0 = dirY - planeY;
            double rayDirX1 = dirX + planeX;
            double rayDirY1 = dirY + planeY;

            int p = y-SCREEN_HEIGHT/2;

            double posZ = 0.5 * SCREEN_HEIGHT;

            double rowDistance = posZ/p;

            double floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_WIDTH;
            double floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_WIDTH;
            double floorX = playerX + rowDistance * rayDirX0;
            double floorY = playerY + rowDistance * rayDirY0;

            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                int cellX = (int)(floorX);
                int cellY = (int)(floorY);
                int tx = (int)(TEXTURE_WIDTH * (floorX - (double)cellX))& (TEXTURE_WIDTH-1);
                int ty = (int)(TEXTURE_HEIGHT * (floorY - (double)cellY))& (TEXTURE_HEIGHT-1);
                floorX += floorStepX;
                floorY += floorStepY;
                int floor_texture = 5;
                int ceilig_texture = 10;
                Uint32 pixel;

                pixel = textures[floor_texture][TEXTURE_WIDTH*ty + tx];
                pixel = (pixel >>1 )& 8355711;
                buff[y][x] = pixel;

                pixel = textures[ceilig_texture][TEXTURE_WIDTH*ty + tx];
                pixel = (pixel >>1) & 8355711;
                buff[SCREEN_HEIGHT - y - 1][x] = pixel;
            }
        }

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

            int texNum = map[mapX][mapY];

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
        }

        SDL_RenderClear(renderer);

        for (int x = 0; x < SCREEN_WIDTH; x++) {
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
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
    IMG_Quit();

    for (int i = 0; i < TEXTURES_NUMBER; i++) {
        free(textures[i]);
    }
    free(textures);
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        free(buff[i]);
    }
    free(buff);
    return 0;
}