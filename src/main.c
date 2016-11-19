#include <stdio.h>
#include "config.h"
#include "gpu.h"
#include "cpu.h" 
#include <stdlib.h>
#include <unistd.h>
#include <agar/core.h>
#include <agar/gui.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <SDL/SDL.h>
#include <gtk/gtk.h>
#include "gui.h"
#include "gui/window-debugger.h"

#define ORIG_WIDTH 160
#define ORIG_HEIGHT 144
#define SCREEN_SCALE 2

#define DEBUG_WIDTH 300
#define SCREEN_WIDTH (ORIG_WIDTH*SCREEN_SCALE)
#define INFO_WIDTH 300
#define SCREEN_HEIGHT (ORIG_HEIGHT*SCREEN_SCALE)
#define DEBUG_HEIGHT 100
#define CONSOLE_HEIGHT 200

void doGUI();
void consoleLog(const char *format, ...);
void consoleError(const char *format, ...);
void startTimer();
void drawScreen();
void dumpMem();
void eventCallback();
void createTilesWindow();
int surfaceIndex;
AG_Pixmap *mainPM;
AG_Surface *mainSurface;
SDL_Surface *sdlSurface;

uint32_t fullMap[ORIG_WIDTH*ORIG_HEIGHT];

AG_Window *win;
AG_Timer timer;
AG_Console *cons;
AG_Color errorColor = {255, 0, 0}; 
AG_Surface *mainSurface;
AG_Surface *scaledSurface;
AG_Pixmap *mainPM;
AG_Window *pWin;
AG_Surface *pSurface;
AG_Pixmap *pPM;

size_t PrintX16(AG_FmtString *fs, char *dst, size_t dstSize){
	uint16_t *var = AG_FMTSTRING_ARG(fs);
    sprintf(dst, "%04X", *var);
    return strlen(dst);
}

size_t PrintX8(AG_FmtString *fs, char *dst, size_t dstSize){
	uint8_t *var = AG_FMTSTRING_ARG(fs);
    sprintf(dst, "%02X", *var);
    return strlen(dst);
}



void toggleExecution();

int main(int argc, char **argv){
    printf("%s: Version %d.%d\n", GBEmu_NAME, GBEmu_VERSION_MAJOR, GBEmu_VERSION_MINOR);
    cpu.running = 0;
    initCPU();
    // Quick checker
    int sum = 0;
    for(int i = 0; i < 256; i++){
        sum += opcode_implemented[i];
        if (!opcode_implemented[i]){
            printf("Not Implemented: 0x%02X\n", i);
        }
    }
    printf("Completed %d/%d\n", sum, 256-11);
    FILE *rom = fopen(argv[2], "rb");
    readFile(0, rom);
    fclose(rom);
    FILE *bios = fopen(argv[1], "rb");
    readFile(0, bios);
    fclose(bios);
    clearTiles();
    cpu.PC = 0x100;
    //cpu.PC = 0x00;
    

    launchGUI("HelloWorld.glade", argc, argv);
    return 0;
}
void eventCallback(){
    printf("Event called\n");
}
void doGUI(){
}

/*
void doGUI(){
    char* test = "POO";
    if (AG_InitCore(NULL, 0) == -1 ||
        AG_InitGraphics(0) == -1) {
		fprintf(stderr, "Init failed: %s\n", AG_GetError());
		return;
	}
    // Create our window
    win = AG_WindowNewNamed(0, GBEmu_NAME);

    // Setup boxes
    AG_Fixed * container = AG_FixedNew(win, AG_FIXED_EXPAND);
    AG_Box *debugBox = AG_BoxNew(container, AG_BOX_VERT, AG_BOX_FRAME| AG_BOX_EXPAND);
    AG_Box *consoleBox = AG_BoxNew(container, AG_BOX_VERT, AG_BOX_FRAME| AG_BOX_EXPAND);
    AG_Box *gameBox = AG_BoxNew(container, AG_BOX_VERT, AG_BOX_FRAME| AG_BOX_EXPAND);
    AG_Box *infoBox = AG_BoxNew(container, AG_BOX_VERT, AG_BOX_FRAME| AG_BOX_EXPAND);
    AG_FixedMove(container, debugBox, 0, 0);
    AG_FixedSize(container, debugBox, DEBUG_WIDTH, DEBUG_HEIGHT);
    AG_FixedMove(container, consoleBox, 0, DEBUG_HEIGHT);
    AG_FixedSize(container, consoleBox, DEBUG_WIDTH, CONSOLE_HEIGHT);
    AG_FixedMove(container, gameBox, DEBUG_WIDTH, 0);
    AG_FixedSize(container, gameBox, SCREEN_WIDTH, SCREEN_HEIGHT);
    AG_FixedMove(container, infoBox, DEBUG_WIDTH+SCREEN_WIDTH, 0);
    AG_FixedSize(container, infoBox, INFO_WIDTH, DEBUG_HEIGHT+CONSOLE_HEIGHT);

    // Setup pixel rendered
    
    mainSurface = AG_SurfaceFromPixelsRGB(fullMap, ORIG_WIDTH, ORIG_HEIGHT, 32, 0xFF0000, 0x00FF00, 0x0000FF);
    scaledSurface = AG_SurfaceRGB(SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0xFF0000, 0x00FF00, 0x0000FF);

    mainPM = AG_PixmapFromSurfaceNODUP(gameBox, AG_PIXMAP_RESCALE, scaledSurface);
    //mainPM = AG_PixmapFromSurface(gameBox, AG_PIXMAP_EXPAND, mainSurface);

    // Register custom formatter
    AG_RegisterFmtStringExt("x16", PrintX16);
    AG_RegisterFmtStringExt("x8", PrintX8);

    // Setup flag options first 
    AG_CheckboxNewInt(infoBox, 0, "Zero", &cpu.F_Z);
    AG_CheckboxNewInt(infoBox, 0, "Negative", &cpu.F_N);
    AG_CheckboxNewInt(infoBox, 0, "Half Carry", &cpu.F_H);
    AG_CheckboxNewInt(infoBox, 0, "Carry", &cpu.F_C);
    // PC log
    cons = AG_ConsoleNew(consoleBox, AG_CONSOLE_EXPAND);

    AG_ButtonNewFn(debugBox, 0, "Step", executeOpcode, NULL);
    AG_ButtonNewFn(debugBox, 0, "Draw", drawScreen, NULL);
    
    AG_ButtonNewFn(debugBox, 0, "Dump", dumpMem, NULL);
    AG_ButtonNewFn(debugBox, 0, "Tiles", createTilesWindow, NULL);
    AG_ButtonNewFn(debugBox, AG_BUTTON_STICKY, "Run", toggleExecution, NULL);
    AG_Label *lbl = AG_LabelNewPolled(infoBox, 0, "PC: 0x%[x16]", &cpu.PC);
    AG_LabelSizeHint(lbl, 1, "PC: 0xXXXX");
    lbl = AG_LabelNewPolled(infoBox, 0, "SP: 0x%[x16]", &cpu.SP);
    AG_LabelSizeHint(lbl, 1, "PC: 0xXXXX");
    lbl = AG_LabelNewPolled(infoBox, 0, "A: 0x%[x8]", &cpu.A);
    AG_LabelSizeHint(lbl, 1, "C: 0xXXXX");
    lbl = AG_LabelNewPolled(infoBox, 0, "B: 0x%[x8]", &cpu.B);
    AG_LabelSizeHint(lbl, 1, "C: 0xXXXX");
    lbl = AG_LabelNewPolled(infoBox, 0, "C: 0x%[x8]", &cpu.C);
    AG_LabelSizeHint(lbl, 1, "C: 0xXXXX");
    lbl = AG_LabelNewPolled(infoBox, 0, "D: 0x%[x8]", &cpu.D);
    AG_LabelSizeHint(lbl, 1, "C: 0xXXXX");
    lbl = AG_LabelNewPolled(infoBox, 0, "E: 0x%[x8]", &cpu.E);
    AG_LabelSizeHint(lbl, 1, "C: 0xXXXX");
    lbl = AG_LabelNewPolled(infoBox, 0, "HL: 0x%[x16]", &cpu.HL);
    AG_LabelSizeHint(lbl, 1, "PC: 0xXXXX");
    AG_WindowUpdate(win);
    
    AG_WindowSetPadding(win, 0, 0, 0, 0);
    AG_WindowSetGeometryAligned(win, AG_WINDOW_MC, DEBUG_WIDTH+SCREEN_WIDTH+INFO_WIDTH, DEBUG_HEIGHT+CONSOLE_HEIGHT);
    AG_WindowShow(win);
    AG_EventLoop();
    return;
}
*/

void toggleExecution(){
    running = !running;
}

// Log to console instead
void consoleLog(const char *format, ...){
    va_list argptr;
    va_start(argptr, format);
    vprintf(format, argptr);
    va_end(argptr);
}

void consoleError(const char *format, ...){
    va_list argptr;
    va_start(argptr, format);
    vprintf(format, argptr);
    va_end(argptr);
}



void drawScreen(){
    uint32_t *pixels = (uint32_t *)mainSurface->pixels;
    for(int i = 0; i < ORIG_WIDTH*ORIG_HEIGHT; i++){
        pixels[i] = fullMap[i];
    }
    /*
    for(int x = 0; x < ORIG_WIDTH; x++){
        for(int y = 0; y < ORIG_HEIGHT; y++){
            AG_PUT_PIXEL2(mainSurface, x, y, x%2?0xFF00FF:0x00FF00);
        }
    }
    */
    /*
    for(int i = 0; i < 8; i++){
        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                uint32_t colour;
                switch(tiles[i][x][y]){
                    case 0x0:
                        colour = 0xFFFFFF;
                        break;
                    case 0x1:
                        colour = 0xFF0000;
                        break;
                    case 0x2:
                        colour = 0x00FF00;
                        break;
                    case 0x3:
                        colour = 0x0000FF;
                        break;
                }
                AG_PUT_PIXEL2(mainSurface, 8*i+x, y, colour);
            }
        }
    }*/
    AG_ScaleSurface(mainSurface, SCREEN_WIDTH, SCREEN_HEIGHT, &scaledSurface);
    AG_PixmapUpdateSurface(mainPM, mainPM->n);
    AG_WindowUpdate(win);

}

void createTilesWindow(){
    int tilesWide = 16;
    int tilesTall = 24;

    uint32_t tileMap[tilesWide*tilesTall*8*8]; 

    // Format to proper format
    for(int i = 0; i < tilesWide*tilesTall; i++){
        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                uint32_t colour;
                switch(tiles[i][y][x]){
                    case 0x0:
                        colour = 0xFFFFFF;
                        break;
                    case 0x1:
                        colour = 0xFF0000;
                        break;
                    case 0x2:
                        colour = 0x00FF00;
                        break;
                    case 0x3:
                        colour = 0x0000FF;
                        break;
                }
                uint16_t x1 = (i%tilesWide)*8+x;
                uint16_t y1 = (i/tilesWide)*8+y;
                tileMap[y1*tilesWide*8+x1] = colour;
            }
        }
    }
    

    pWin = AG_WindowNewNamed(0, GBEmu_NAME " - Tiles");
    AG_Box *pBox = AG_BoxNew(pWin, AG_BOX_HORIZ, AG_BOX_EXPAND|AG_BOX_FRAME);
    pSurface = AG_SurfaceFromPixelsRGB(tileMap, tilesWide*8, tilesTall*8, 32, 0xFF0000, 0x00FF00, 0x0000FF);
    AG_Surface *pScaled = AG_SurfaceRGB(tilesWide*8*SCREEN_SCALE, tilesTall*8*SCREEN_SCALE, 32, 0, 0xFF0000, 0x00FF00, 0x0000FF);

    AG_ScaleSurface(pSurface, tilesWide*8*SCREEN_SCALE, tilesTall*8*SCREEN_SCALE, &pScaled);
    pPM = AG_PixmapFromSurfaceNODUP(pBox, AG_PIXMAP_RESCALE, pScaled);

    AG_PixmapUpdateSurface(pPM, pPM->n);
    AG_WindowUpdate(pWin);

}