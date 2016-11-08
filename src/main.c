#include <stdio.h>
#include "config.h"
#include "cpu.h"

int main(int argc, char **argv){
    printf("%s: Version %d.%d\n", GBEmu_NAME, GBEmu_VERSION_MAJOR, GBEmu_VERSION_MINOR);
    initCPU();
    cpu.C = 3;
    cpu.B = 2;
    cpu.PC = 0x41;
    executeOpcode();
    // Quick checker
    int sum = 0;
    for(int i = 0; i < 256; i++){
        sum += opcode_implemented[i];
    }
    printf("Completed %d/%d\n", sum, 256-11);
    return 0;
}