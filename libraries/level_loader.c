#include "level_loader.h"

static int errorValue = ERROR_NONE;

char* get_error_message() {
    switch (errorValue) {
        case ERROR_NONE: return "NO ERROR";
        case ERROR_FILENAME: return "FILENAME ERROR PLEASE TRY AGAIN";
        case ERROR_FILE_OPEN: return "FILE ERROR PLEASE TRY AGAIN";
        case ERROR_MEMORY_ALLOCATION: return "MEMORY ALLOCATION ERROR";
        case ERROR_READING_HEADER: return "ERROR READING MAP HEADER";
        case ERROR_READING_TILES: return "ERROR READING MAP TILES";
        default: return "UNKNOWN ERROR";
    }
}

map_info_t* loadMap(const char* filename) {
    if (filename == NULL) {
        errorValue = ERROR_FILENAME;
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        errorValue = ERROR_FILE_OPEN;
        return NULL;
    }

    map_info_t* map = (map_info_t*)malloc(sizeof(map_info_t));
    if (map == NULL) {
        fclose(file);
        errorValue = ERROR_MEMORY_ALLOCATION;
        return NULL;
    }

    if (fscanf(file, "%d %d", &map->width, &map->height) != 2) {
        free(map);
        fclose(file);
        errorValue = ERROR_READING_HEADER;
        return NULL;
    }

    if (fscanf(file, "%hhu %hhu", &map->ceilingTextureNumber, &map->floorTextureNumber) != 2) {
        free(map);
        fclose(file);
        errorValue = ERROR_READING_HEADER;
        return NULL;
    }

    if (fscanf(file, "%hhu %hhu", &map->playerStartingTileX, &map->playerStartingTileY) != 2) {
        free(map);
        fclose(file);
        errorValue = ERROR_READING_HEADER;
        return NULL;
    }

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            int value;
            if (fscanf(file, "%d", &value) != 1) {
                free(map);
                fclose(file);
                errorValue = ERROR_READING_TILES;
                return NULL;
            }
            map->value_map[x][y] = (tile)value;
        }
    }

    fclose(file);
    errorValue = ERROR_NONE;
    return map;
}

int saveMap(const char* filename, map_info_t* map) {
    if (filename == NULL || map == NULL) {
        errorValue = ERROR_FILENAME;
        return 0;
    }

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        errorValue = ERROR_FILE_OPEN;
        return 0;
    }

    fprintf(file, "%d %d\n", map->width, map->height);
    fprintf(file, "%hhu %hhu\n", map->ceilingTextureNumber, map->floorTextureNumber);
    fprintf(file, "%hhu %hhu\n", map->playerStartingTileX, map->playerStartingTileY);

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            fprintf(file, "%d ", map->value_map[x][y]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    errorValue = ERROR_NONE;
    return 1;
}

void freeMap(map_info_t* map) {
    if (map) {
        free(map);
    }
}
