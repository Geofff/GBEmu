#include <stdio.h>
#include "config.h"
#include "gpu.h"
#include "cpu.h" 
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
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
