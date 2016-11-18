#pragma once
#include "cpu.h"

typedef enum {OAM_mode, VRAM_mode, HBLANK_mode, VBLANK_mode} GPU_mode;
typedef struct GPU{
    uint16_t tick;
    GPU_mode mode;
    uint16_t line;
    uint8_t draw;
    uint8_t bgMode;
    uint8_t bgTile;
    uint8_t bgMap;
    uint8_t switchLCD;
    uint8_t scanX;
    uint8_t scanY;
    uint8_t *pallete;
    
} GPU;

GPU gpu;
uint8_t tiles[384][8][8];

void clearTiles();
void gpuTick();
void updateTileSet(uint16_t address, uint8_t value);
void renderScanline();