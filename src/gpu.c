#include "gpu.h"

void updateTileSet(uint16_t address, uint8_t value){
    address &= 0x1FFE;
    uint16_t tile = (address >> 4) & 511;
    printf("New address is 0x%04X\t tile is 0x%X\n", address, tile);
    uint8_t y = (address >> 1) & 7;
    uint8_t sx = 0;
    for(int x = 0 ; x < 8; x++){
        sx = 1 << (7-x);
        tiles[tile][y][x] = ((VRAM[address] & sx) ? 1 : 0) + ((VRAM[address + 1] & sx) ? 2 : 0);
    }
    printf("Updated tiles\n");
    gpu.draw = 1;
}
void clearTiles(){

    gpu.pallete[0] = 0xFFFFFF;
    gpu.pallete[1] = 0xFF0000;
    gpu.pallete[2] = 0xFFFF00;
    gpu.pallete[3] = 0xFFFFFF;
    for(int i = 0; i < 384;i++){
        for(int x = 0; x < 8; x++){
            for (int y = 0; y < 8; y++){
                tiles[i][y][x] = 0;
            }
        }
    }
}
void renderScanline(){
    uint16_t mapOffset = gpu.bgMode ? 0x1C00 : 0x1800;
    mapOffset += ((gpu.line + gpu.scanY)&255) >> 3;
    uint16_t lineOffset = gpu.scanX >> 3;
    uint8_t x = gpu.scanX & 7;
    uint8_t y = (gpu.line + gpu.scanY) & 7;
    uint16_t canvasOffset = gpu.line * 160;
    uint8_t tile = VRAM[mapOffset+lineOffset];
    if (gpu.bgMode && tile < 128){
        tile += 256;
    }
    for(int i = 0; i < 160; i++){
        printf("Plotting colour %d\n", i);
        switch(tiles[i][y][x]){
            case 0x0:
                fullMap[canvasOffset+0] = 0xFF;
                fullMap[canvasOffset+1] = 0xFF;
                fullMap[canvasOffset+2] = 0xFF;
                fullMap[canvasOffset+3] = 0xFF;
                break;
            case 0x1:
                fullMap[canvasOffset+0] = 0xFF;
                fullMap[canvasOffset+1] = 0x00;
                fullMap[canvasOffset+2] = 0x00;
                fullMap[canvasOffset+3] = 0xFF;
                break;
            case 0x2:
                fullMap[canvasOffset+0] = 0x00;
                fullMap[canvasOffset+1] = 0xFF;
                fullMap[canvasOffset+2] = 0x00;
                fullMap[canvasOffset+3] = 0xFF;
                break;
            case 0x3:
                fullMap[canvasOffset+0] = 0x00;
                fullMap[canvasOffset+1] = 0x00;
                fullMap[canvasOffset+2] = 0xFF;
                fullMap[canvasOffset+3] = 0xFF;
                break;
        }
        canvasOffset+=4;
        x++;
        if (x == 8){
            x = 0;
            lineOffset = (lineOffset+1)&31;
            tile = VRAM[mapOffset+lineOffset];
            if (gpu.bgMode && tile < 128){
                tile += 256;
            }
        }
    }
    gpu.draw = 1;

}

void gpuTick(){
    gpu.tick += opcode_length[opcode];
    switch(gpu.mode){
        case OAM_mode:
            if (gpu.tick >= 80){
                gpu.tick = 0;
                gpu.mode = VRAM_mode;
            }
            break;
        case VRAM_mode:
            if (gpu.tick >= 172){
                gpu.tick = 0;
                gpu.mode = HBLANK_mode;
            }
            break;
        case HBLANK_mode:
            if (gpu.tick >= 204){
                gpu.tick = 0;
                gpu.line++;
                if (gpu.line == 143){
                    gpu.mode = VBLANK_mode;
                    renderScanline();
                } else {
                    gpu.mode = OAM_mode;
                }
            }
            break;
        case VBLANK_mode:
            if (gpu.tick >= 456){
                gpu.tick = 0;
                gpu.line++;
                if (gpu.line > 153){
                    gpu.mode = OAM_mode;
                    gpu.line = 0;
                }
            }
            break;
    }

}