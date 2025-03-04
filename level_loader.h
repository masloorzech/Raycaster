//
// Created by anton on 04.03.2025.
//

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXIMUM_MAP_WIDTH 128
#define MAXIMUM_MAP_HEIGHT 128

typedef uint8_t tile;

enum {
    ERROR_NONE = 0,
    ERROR_FILENAME = 1,
    ERROR_FILE_OPEN = 2,
    ERROR_MEMORY_ALLOCATION = 3,
    ERROR_READING_HEADER = 4,
    ERROR_READING_MAP = 5,
    ERROR_READING_TILES=6
};


typedef struct map_info {
    int width;
    int height;
    uint8_t ceilingTextureNumber;
    uint8_t floorTextureNumber;
    tile playerStartingTileX;
    tile playerStartingTileY;
    tile value_map[MAXIMUM_MAP_WIDTH][MAXIMUM_MAP_HEIGHT];
}map_info_t;

/*
 *FILE STRUCTURE:
 *
 *width height
 *ceilingTexture floorTexture
 *PlayerPosX PlayerPosY
 *0 0 0 0 ...
 *0 0 0 0 ...
 *0 0 0 0 ...
 *. . . . ...
 *
 */

char* get_error_message();
map_info_t* loadMap(const char* filename);
int saveMap(const char* filename, map_info_t* map_info);
void freeMap(map_info_t* map);


#endif //LEVEL_LOADER_H
