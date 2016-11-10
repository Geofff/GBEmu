#include <stdio.h>
#include "config.h"
#include "cpu.h" 
#include <stdlib.h>
#include <unistd.h>
#include <agar/core.h>
#include <agar/gui.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <SDL2/SDL.h>

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
void draw();
int surfaceIndex;

uint32_t fullMap[ORIG_WIDTH*ORIG_HEIGHT];

AG_Window *win;
AG_Timer timer;
AG_Console *cons;
AG_Color errorColor = {255, 0, 0}; 
AG_Surface *mainSurface;
AG_Pixmap *mainPM;

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

void *executorFunc(){
    printf("got launched!");
    while (1){
        usleep(250000);
        if (running){
            executeOpcode();
        }
    }
}

void toggleExecution();
int main(int argc, char **argv){
    pthread_t pth;

    pthread_create(&pth, NULL, &executorFunc, NULL);
    printf("%s: Version %d.%d\n", GBEmu_NAME, GBEmu_VERSION_MAJOR, GBEmu_VERSION_MINOR);
    running = 0;
    initCPU();
    // Quick checker
    int sum = 0;
    for(int i = 0; i < 256; i++){
        sum += opcode_implemented[i];
    }
    consoleLog("Completed %d/%d\n", sum, 256-11);
    FILE *bios = fopen(argv[1], "rb");
    readFile(BIOS, bios);
    fclose(bios);
    FILE *rom = fopen(argv[2], "rb");
    readFile(ROM, rom);
    fclose(rom);
    cpu.PC = 0x100;
    
    doGUI();
    return 0;
}

void doGUI(){
    char* test = "POO";
    if (AG_InitCore(NULL, 0) == -1 ||
        AG_InitGraphics(0) == -1) {
		fprintf(stderr, "Init failed: %s\n", AG_GetError());
		return;
	}
    // Create our window
    win = AG_WindowNew(0);

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

    mainPM = AG_PixmapFromSurfaceScaled(gameBox, AG_PIXMAP_RESCALE, mainSurface, SCREEN_WIDTH, SCREEN_HEIGHT);
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
    AG_ButtonNewFn(debugBox, 0, "Draw", draw, NULL);
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
    
    AG_WindowSetPadding(win, 0, 0, 0, 0);
    AG_WindowSetGeometryAligned(win, AG_WINDOW_MC, DEBUG_WIDTH+SCREEN_WIDTH+INFO_WIDTH, DEBUG_HEIGHT+CONSOLE_HEIGHT);
    AG_WindowShow(win);
    AG_EventLoop();
    return;
}

void toggleExecution(){
    running = !running;
}

// Log to console instead
void consoleLog(const char *format, ...){
    va_list argptr;
    va_start(argptr, format);
    // Naughty buffer overflow
    char formatted[256];
    vsprintf(formatted, format, argptr);
    va_end(argptr);
    AG_ConsoleMsg(cons, "%s", formatted);
}

void consoleError(const char *format, ...){
    va_list argptr;
    va_start(argptr, format);
    // Naughty buffer overflow
    char formatted[256];
    vsprintf(formatted, format, argptr);
    va_end(argptr);
    AG_ConsoleLine *line = AG_ConsoleMsg(cons, "%s", formatted);
    AG_ConsoleMsgColor(line, &errorColor);
}

void draw(){
    for(int x = 0; x < ORIG_WIDTH; x++){
        for(int y = 0; y < ORIG_HEIGHT; y++){
            AG_PUT_PIXEL2(mainSurface, x, y, 0xFF00FF);
        }
    }
    AG_PixmapUpdateSurface(mainPM, mainPM->n);

}