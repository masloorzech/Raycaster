#include <math.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "libraries/level_loader.h"
#include <pthread.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define SCALE_FACTOR (4)

#define RENDER_WIDTH (int)(SCREEN_WIDTH/(double)SCALE_FACTOR)
#define RENDER_HEIGHT (int)(SCREEN_HEIGHT/(double)SCALE_FACTOR)
#define WINDOW_NAME "SIMPLE RAYCATER GAME"

#define TEXTURES_NUMBER 64
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

#define BASIC_SPEED 5.0
#define SPRINT_SPEED 7.0

enum game_states {
    RUNNING,
    PAUSED,
    QUIT
};

typedef struct player_keymap{
    bool moveForward;
    bool moveBackward;
    bool moveLeft;
    bool moveRight;
    bool sprint;
}player_keymap_t;

typedef struct camera {
    double planeX;
    double planeY;
}camera_t;

typedef struct player {
    double playerX;
    double playerY;
    double dirX;
    double dirY;
    double angle;
    double speed;
    camera_t camera;
    player_keymap_t keymap;
}player_t;

typedef struct {
    int startX;
    int endX;
    double height;
    const player_t* player;
    map_info_t *map;
    Uint32 **textures;
    Uint32 *buffer;
}thread_data_t;

int load_texture(const char* file_path, Uint32* texture) {
    SDL_Surface* surface = IMG_Load(file_path);
    if (!surface) {
        printf("Cannot load image: %s\n", IMG_GetError());
        return -1;
    }

    if (surface->w != TEXTURE_WIDTH || surface->h != TEXTURE_HEIGHT) {
        printf("Image has different sizes than expected\n");
        SDL_FreeSurface(surface);
        return -1;
    }

    SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (!converted_surface) {
        printf("Cannot convert image to surface: %s\n", SDL_GetError());
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

Uint32* create_screen_buffer() {
    Uint32* buff = (Uint32*)malloc(RENDER_HEIGHT * RENDER_WIDTH * sizeof(Uint32));
    if (!buff) {
        printf("Allocation error\n");
        return NULL;
    }
    memset(buff, 0, RENDER_HEIGHT * RENDER_WIDTH * sizeof(Uint32));
    return buff;
}

void free_screen_buffer(Uint32* buff) {
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
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/WOOD_1C.PNG",
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/BRICK_2B.PNG",
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/BRICK_1A.PNG",
        "C:/Users/anton/CLionProjects/Raycaster/assets/textures/greystone.PNG"
    };
    for (int i = 0; i < TEXTURES_NUMBER; i++) {
        if (file_paths[i]==NULL){
            continue;
        }
        if (load_texture(file_paths[i], textures[i]) == -1) {
            printf("Cannot load image: %s\n", file_paths[i]);
            return -1;
        }
    }
    return 0;
}

void calculate_floor_and_ceiling(int start_x, int end_x, double height, const player_t* player,map_info_t * map_info, const Uint32** textures, Uint32* buff) {
    for (int y = 0; y < height; y++) {
        double rayDirX0 = player->dirX - player->camera.planeX;
        double rayDirY0 = player->dirY - player->camera.planeY;
        double rayDirX1 = player->dirX + player->camera.planeX;
        double rayDirY1 = player->dirY + player->camera.planeY;

        int p = y-RENDER_HEIGHT/2;

        double posZ = 0.5 * RENDER_HEIGHT;

        double rowDistance = posZ/p;

        double floorStepX = rowDistance * (rayDirX1 - rayDirX0) / RENDER_WIDTH;
        double floorStepY = rowDistance * (rayDirY1 - rayDirY0) / RENDER_WIDTH;
        double floorX = player->playerX + rowDistance * rayDirX0;
        double floorY = player->playerY + rowDistance * rayDirY0;

        for (int x = start_x; x < end_x; ++x) {
            int cellX = (int)(floorX);
            int cellY = (int)(floorY);
            int tx = (int)(TEXTURE_WIDTH * (floorX - (double)cellX))& (TEXTURE_WIDTH-1);
            int ty = (int)(TEXTURE_HEIGHT * (floorY - (double)cellY))& (TEXTURE_HEIGHT-1);
            floorX += floorStepX;
            floorY += floorStepY;
            int floor_texture = map_info->floorTextureNumber;
            int ceilig_texture = map_info->ceilingTextureNumber;

            Uint32 pixel = textures[floor_texture][TEXTURE_WIDTH * ty + tx];
            pixel = (pixel >>1 )& 8355711;
            buff[y * RENDER_WIDTH + x] = pixel;

            pixel = textures[ceilig_texture][TEXTURE_WIDTH*ty + tx];
            pixel = (pixel >>1) & 8355711;
            buff[(RENDER_HEIGHT - y - 1) * RENDER_WIDTH + x] = pixel;
        }
    }
}
void calculate_walls_for_thread(int start_x,int end_x,const double height, const player_t *player,map_info_t*map,Uint32** textures, Uint32* buff) {

    for (int x = start_x; x < end_x; x++) {

            const double cameraX = 2*x / (double)RENDER_WIDTH-1;
            const double rayDirX = player->dirX + player->camera.planeX * cameraX;
            const double rayDirY = player->dirY + player->camera.planeY * cameraX;

            int mapX = (int)player->playerX;
            int mapY = (int)player->playerY;

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
                sideDistX = (player->playerX - mapX) * deltaDistX;
            }
            else {
                stepX += 1;
                sideDistX = (mapX + 1.0 - player->playerX) * deltaDistX;
            }
            if (rayDirY < 0) {
                stepY -= 1;
                sideDistY = (player->playerY - mapY) * deltaDistY;
            }
            else {
                stepY += 1;
                sideDistY = (mapY + 1.0 - player->playerY) * deltaDistY;
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
                if (map->value_map[mapX][mapY] > 0) {
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

            const int texNum = map->value_map[mapX][mapY];

            double wallX =0;
            if (side == 0) {
                wallX = player->playerY + perpWallDist * rayDirY;
            }else {
                wallX = player->playerX + perpWallDist * rayDirX;
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
                buff[y * RENDER_WIDTH + x] = color;
            }
        }
}

void* calculate_walls_segments(void* arg) {
    thread_data_t* thread_data = (thread_data_t*)arg;
    int start_x = thread_data->startX;
    int end_x = thread_data->endX;
    double height = thread_data->height;
    const player_t *player = thread_data->player;
    map_info_t* map = thread_data->map;
    Uint32** textures = thread_data->textures;
    Uint32* buff = thread_data->buffer;
    calculate_walls_for_thread(start_x,end_x,height,player,map,textures,buff);
    return NULL;
}

void calculate_walls(double height, const player_t *player, map_info_t* map, Uint32** textures, Uint32* buff) {
    int num_threads = 4;
    pthread_t threads[num_threads];
    thread_data_t thread_data[num_threads];
    int chunk_size = RENDER_WIDTH / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].startX = i * chunk_size;
        thread_data[i].endX= (i == num_threads - 1) ? RENDER_WIDTH : (i + 1) * chunk_size;
        thread_data[i].height = height;
        thread_data[i].player = player;
        thread_data[i].map = map;
        thread_data[i].textures = textures;
        thread_data[i].buffer = buff;
        pthread_create(&threads[i], NULL, calculate_walls_segments, &thread_data[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}
void handle_movement(player_t *player,map_info_t *map , const double moveSpeed) {
    if (player->keymap.moveForward) {
        if (map->value_map[(int)(player->playerX + player->dirX * moveSpeed)][(int)player->playerY] == 0) {
            player->playerX += player->dirX * moveSpeed;
        }

        if (map->value_map[(int)player->playerX][(int)(player->playerY + player->dirY * moveSpeed)] == 0) {
            player->playerY += player->dirY * moveSpeed;
        }
    }
    if (player->keymap.moveBackward) {
        if (map->value_map[(int)(player->playerX - player->dirX * moveSpeed)][(int)player->playerY] == 0) {
            player->playerX -= player->dirX * moveSpeed;
        }
        if (map->value_map[(int)player->playerX][(int)(player->playerY - player->dirY * moveSpeed)] == 0) {
            player->playerY -= player->dirY * moveSpeed;
        }
    }
    if (player->keymap.moveLeft) {
        if (map->value_map[(int)(player->playerX - player->camera.planeX * moveSpeed)][(int)player->playerY] == 0) {
            player->playerX -= player->camera.planeX * moveSpeed;
        }
        if (map->value_map[(int)player->playerX][(int)(player->playerY - player->camera.planeY * moveSpeed)] == 0) {
            player->playerY -= player->camera.planeY * moveSpeed;
        }
    }

    if (player->keymap.moveRight) {
        if (map->value_map[(int)(player->playerX + player->camera.planeX * moveSpeed)][(int)player->playerY] == 0) {
            player->playerX += player->camera.planeX * moveSpeed;
        }
        if (map->value_map[(int)player->playerX][(int)(player->playerY + player->camera.planeY * moveSpeed)] == 0) {
            player->playerY += player->camera.planeY * moveSpeed;
        }
    }

    if (player->keymap.sprint) {
        player->speed = SPRINT_SPEED;
    }else {
        player->speed = BASIC_SPEED;
    }
}

void handle_mouse_movement(player_t *player) {
    int mouseX=0, mouseY=0;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);

    double mouseSensitivity = 0.002;
    player->angle += mouseX * mouseSensitivity;

    // Zabezpieczenie przed przekroczeniem granic
    if (player->angle < -M_PI) player->angle += 2 * M_PI;
    if (player->angle > M_PI) player->angle -= 2 * M_PI;

    // Zaktualizuj kierunek
    player->dirX = cos(player->angle);
    player->dirY = sin(player->angle);

    // Zaktualizowanie kamery (plane)
    player->camera.planeX = cos(player->angle + M_PI / 2) * 0.66;
    player->camera.planeY = sin(player->angle + M_PI / 2) * 0.66;
}

void handle_keyboard(SDL_Event event,player_t *player,enum game_states *gameState, SDL_Window *window) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
             *gameState = QUIT;
        }
        if (event.type == SDL_KEYDOWN) {
            if (*gameState == RUNNING) {
                if (event.key.keysym.sym == SDLK_w) player->keymap.moveForward = true;
                if (event.key.keysym.sym == SDLK_s) player->keymap.moveBackward = true;
                if (event.key.keysym.sym == SDLK_a) player->keymap.moveLeft = true;
                if (event.key.keysym.sym == SDLK_d) player->keymap.moveRight = true;
                if (event.key.keysym.sym == SDLK_LSHIFT) player->keymap.sprint = true;
            }

            if (event.key.keysym.sym == SDLK_ESCAPE) {*gameState = QUIT;}

            if (event.key.keysym.sym == SDLK_p) {

                SDL_GetRelativeMouseState(NULL, NULL);
                if (*gameState == RUNNING) {
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    *gameState = PAUSED;
                    SDL_WarpMouseInWindow(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
                }else if (*gameState == PAUSED) {
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    *gameState = RUNNING;
                }
            }
        }

        if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_w) player->keymap.moveForward = false;
                if (event.key.keysym.sym == SDLK_s) player->keymap.moveBackward = false;
                if (event.key.keysym.sym == SDLK_a) player->keymap.moveLeft = false;
                if (event.key.keysym.sym == SDLK_d) player->keymap.moveRight = false;
                if (event.key.keysym.sym == SDLK_LSHIFT) player->keymap.sprint = false;
        }
    }
}

int main(int argc, char *argv[]) {

    char path[PATH_MAX];
    path[0] = '\0';
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

    Uint32* buff = create_screen_buffer();
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
    map_info_t* map = loadMap("C:/Users/anton/CLionProjects/Raycaster/maps/Test_map.txt");
    if (map == NULL) {
        printf("%s",get_error_message());
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

    SDL_Event event = {};

    camera_t camera;
    camera.planeX = 0;
    camera.planeY = 0.66;

    player_t player = {
        player.playerX = map->playerStartingTileX,
        player.playerY = map->playerStartingTileY,
        player.dirX = -1.0,
        player.dirY =  0.0,
        player.angle = -M_PI / 2,
        player.speed = BASIC_SPEED,
        player.camera = camera,
        player.keymap.moveForward = false,
        player.keymap.moveBackward = false,
        player.keymap.moveLeft = false,
        player.keymap.moveRight = false,
    };

    double time = 0.0;

    double h = (double)RENDER_HEIGHT;
    clock_t start, end;
    double elapsed_time;
    enum game_states game_state = RUNNING;
    while (game_state == RUNNING || game_state == PAUSED) {

        start = clock();
        calculate_floor_and_ceiling(0,RENDER_WIDTH,h,&player,map,textures,buff);
        calculate_walls(h, &player,map, textures,  buff);
        end = clock();

        elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
        printf("Czas rysowania scian: %.3f ms\n", elapsed_time);

        SDL_SetRenderTarget(renderer, lowResTexture);

        for (int x = 0; x < RENDER_WIDTH; x++) {
            for (int y = 0; y < RENDER_HEIGHT; y++) {
                const uint32_t color = buff[y * RENDER_WIDTH + x];
                SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_SetRenderTarget(renderer, NULL);

        SDL_RenderClear(renderer);

        SDL_Rect destRect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, lowResTexture, NULL, &destRect);

        SDL_RenderPresent(renderer);

        memset(buff, 0, RENDER_HEIGHT * RENDER_WIDTH * sizeof(Uint32));

        double oldTime = time;
        time = SDL_GetTicks();
        double frameTime = (time - oldTime) / 1000.0;

        double moveSpeed = frameTime * player.speed;

        handle_keyboard(event,&player,&game_state,main_window);
        if (game_state == RUNNING) {
            handle_movement(&player,map,moveSpeed);
            handle_mouse_movement(&player);
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
    free(buff);
    freeMap(map);
    return 0;
}