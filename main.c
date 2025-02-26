#include <math.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define RENDER_WIDTH 640
#define RENDER_HEIGHT 480
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

Uint32** create_screen_buffer() {
    Uint32** buff = (Uint32**)malloc(RENDER_HEIGHT * sizeof(Uint32*));
    if (!buff) {
        printf("Błąd alokacji pamięci!\n");
        return NULL;
    }
    for (int i = 0; i < RENDER_HEIGHT; i++) {
        buff[i] = (Uint32*)malloc(RENDER_WIDTH * sizeof(Uint32));
        if (!buff[i]) {
            printf("Błąd alokacji pamięci dla wiersza %d!\n", i);
            for (int j = 0; j < i; j++) {
                free(buff[j]);
            }
            free(buff);
            return NULL;
        }
    }
    return buff;
}
void free_screen_buffer(Uint32** buff) {
    for (int i = 0; i < RENDER_HEIGHT; i++) {
        free(buff[i]);
    }
    free(buff);
}

Uint32** create_textures_buffer() {
    Uint32 **textures = (Uint32 **) malloc(TEXTURES_NUMBER * sizeof(Uint32 *));
    if (textures == NULL) {
        return NULL;
    }
    for (int i = 0; i < TEXTURES_NUMBER; i++) {
        textures[i] = (Uint32*)calloc(TEXTURE_WIDTH * TEXTURE_HEIGHT ,sizeof(Uint32));
        if (textures[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(textures[j]);
            }
            free(textures);
            return NULL;
        }
    }
    return textures;
}
void free_textures_buffer(Uint32** textures) {
    for (int i = 0; i < TEXTURES_NUMBER; i++) {
        free(textures[i]);
    }
    free(textures);
}

void close_libraries() {
    IMG_Quit();
    SDL_Quit();
}

int load_assets(Uint32** textures) {
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
            return -1;
        }
    }
    return 0;
}

void calculate_floor_and_ceiling(const double height, const double dirX , const double dirY, const double planeX, const double planeY, const double playerX, const double playerY, Uint32** textures, Uint32** buff) {
    for (int y = 0; y < height; y++) {

        double rayDirX0 = dirX - planeX;
        double rayDirY0 = dirY - planeY;
        double rayDirX1 = dirX + planeX;
        double rayDirY1 = dirY + planeY;

        int p = y-RENDER_HEIGHT/2;

        double posZ = 0.5 * RENDER_HEIGHT;

        double rowDistance = posZ/p;

        double floorStepX = rowDistance * (rayDirX1 - rayDirX0) / RENDER_WIDTH;
        double floorStepY = rowDistance * (rayDirY1 - rayDirY0) / RENDER_WIDTH;
        double floorX = playerX + rowDistance * rayDirX0;
        double floorY = playerY + rowDistance * rayDirY0;

        for (int x = 0; x < RENDER_WIDTH; ++x) {
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
            buff[RENDER_HEIGHT - y - 1][x] = pixel;
        }
    }
}

void calculate_walls(double height, double dirX , double dirY, double planeX, double planeY, double playerX, double playerY, Uint32** textures, Uint32** buff) {
    for (int x = 0; x < RENDER_WIDTH; x++) {

            const double cameraX = 2*x / (double)RENDER_WIDTH -1;
            const double rayDirX = dirX + planeX * cameraX;
            const double rayDirY = dirY + planeY * cameraX;

            int mapX = (int)playerX;
            int mapY = (int)playerY;

            double sideDistX =0;
            double sideDistY =0;

            const double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
            const double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);

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
            int lineHeight = (int)(height/perpWallDist);
            int drawStart = (int)(-lineHeight/2.0 + height/2.0);
            if (drawStart < 0) {
                drawStart = 0;
            }
            int drawEnd = (int)(lineHeight/2.0 + height/2.0);
            if (drawEnd >= height) {
                drawEnd = (int)(height-1.0);
            }

            const int texNum = map[mapX][mapY];

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

            double texPos = (drawStart - height/2.0 + lineHeight/2.0) * step;
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
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING)!=0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    Uint32** buff = create_screen_buffer();
    if (!buff) {
        close_libraries();
        return -1;
    }

    Uint32** textures = create_textures_buffer();
    if (!textures) {
        close_libraries();
        free_screen_buffer(buff);
        return -1;
    }
    if (load_assets(textures)!=0) {
        free_textures_buffer(textures);
        free_screen_buffer(buff);
        close_libraries();
        return -1;
    }

    SDL_Window* main_window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
    if (main_window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        close_libraries();
        free_screen_buffer(buff);
        free_textures_buffer(textures);
        return -1;
    }

    SDL_Renderer * renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        close_libraries();
        free_screen_buffer(buff);
        free_textures_buffer(textures);
        return -1;
    }
    SDL_Texture *lowResTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);

    bool moveForward = false, moveBackward = false;
    bool moveLeft = false, moveRight = false;

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

    double h = (double)RENDER_HEIGHT;

    double playerAngle = -M_PI / 2;
    bool paused = false;
    while (run) {
        if (!paused) {
            calculate_floor_and_ceiling(h,  dirX ,  dirY,  planeX,  planeY,  playerX,  playerY,  textures, buff);
            calculate_walls(h, dirX , dirY, planeX,  planeY,  playerX,  playerY,  textures,  buff);
        }

        SDL_SetRenderTarget(renderer, lowResTexture);

        for (int x = 0; x < RENDER_WIDTH; x++) {
            for (int y = 0; y < RENDER_HEIGHT; y++) {
                const uint32_t color = buff[y][x];
                SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_SetRenderTarget(renderer, NULL);

        SDL_RenderClear(renderer);

        SDL_Rect destRect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, lowResTexture, NULL, &destRect);

        SDL_RenderPresent(renderer);

        for (int i = 0; i < RENDER_HEIGHT; i++) {
            for (int j = 0; j < RENDER_WIDTH; j++) {
                buff[i][j] = 0;
            }
        }

        double oldTime = time;
        time = SDL_GetTicks();
        double frameTime = (time - oldTime) / 1000.0;

        double moveSpeed = frameTime * 5.0;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (!paused) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) run = false;
                    if (event.key.keysym.sym == SDLK_w) moveForward = true;
                    if (event.key.keysym.sym == SDLK_s) moveBackward = true;
                    if (event.key.keysym.sym == SDLK_a) moveLeft = true;
                    if (event.key.keysym.sym == SDLK_d) moveRight = true;
                }
                if (event.key.keysym.sym == SDLK_p) {
                    if (SDL_GetRelativeMouseMode()==SDL_TRUE) {
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                        paused = !paused;
                    }else if (SDL_GetRelativeMouseMode()==SDL_FALSE) {
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                        paused = !paused;
                    }
                }
            }
            if (event.type == SDL_KEYUP) {
                if (!paused) {
                    if (event.key.keysym.sym == SDLK_w) moveForward = false;
                    if (event.key.keysym.sym == SDLK_s) moveBackward = false;
                    if (event.key.keysym.sym == SDLK_a) moveLeft = false;
                    if (event.key.keysym.sym == SDLK_d) moveRight = false;
                }
            }
            if (!paused) {
                int mouseX=0, mouseY=0;
                SDL_GetRelativeMouseState(&mouseX, &mouseY);

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
    for (int i = 0; i < RENDER_HEIGHT; i++) {
        free(buff[i]);
    }
    free(buff);
    return 0;
}