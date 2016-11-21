#pragma once
#include <stdint.h>
#include <stdio.h>
#include "memory.h"
#define TICKS_PER_S 4190000
#define TICKS_PER_MS (TICKS_PER_S/1000)
#define TICKS_PER_US (TICKS_PER_MS/1000)

typedef struct CPU {
    uint16_t PC;
    uint16_t SP;
    uint8_t A;
    union {
        struct{
            uint8_t C;
            uint8_t B;
        };
        uint16_t BC;
    };
    union {
        struct{
            uint8_t E;
            uint8_t D;
        };
        uint16_t DE;
    };
    union {
        struct{
            uint8_t H;
            uint8_t L;
        };
        uint16_t HL;
    };
    int F_C;
    int F_H;
    int F_N;
    int F_Z;
    uint16_t d16;
    uint8_t d8;
    uint8_t running;
    uint32_t ticks;
} CPU;

typedef struct Interrupt {
    uint8_t enabled;
} Interrupt;

void (*opcode_handlers[256])();
char *opcode_names[256];
int opcode_cycles[256];
int opcode_implemented[256];
int opcode_length[256];
void *opcode_args_1[256];
void *opcode_args_2[256];
uint8_t running;
void executeOpcode();
uint8_t opcode;
// Macro to register opcode
// Args: (Opcode, Num cycles, handler function, opcode name, first argument, second argument)
#define REGISTER_OPCODE(o, c, l, h, n, a, b) {opcode_length[0x##o] = l; opcode_cycles[0x##o] = c; opcode_handlers[0x##o] = code_handler_##h; opcode_names[0x##o] = n; opcode_implemented[0x##o] = 1; opcode_args_1[0x##o] = (void*)a;opcode_args_2[0x##o] = b;}
//#define SET_ZN(value) cpu.F_Z = !(value); cpu.F_N = 
CPU cpu;
Interrupt interrupts;
void initCPU();


extern void consoleLog(const char *format, ...);
extern void consoleError(const char *format, ...);
