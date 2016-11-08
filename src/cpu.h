#include <stdint.h>
#include <stdio.h>

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
    union {
        struct {
            unsigned int F_UNUSED_1 : 1;
            unsigned int F_UNUSED_2 : 1;
            unsigned int F_UNUSED_3 : 1;
            unsigned int F_UNUSED_4 : 1;
            unsigned int F_C : 1;
            unsigned int F_H : 1;
            unsigned int F_N : 1;
            unsigned int F_Z : 1;
        };
        uint8_t F;
    };
    uint8_t *HL_VAL;
    uint16_t d16;
    uint8_t d8;
} CPU;

void (*opcode_handlers[256])();
char *opcode_names[256];
int opcode_cycles[256];
int opcode_implemented[256];
void *opcode_args_1[256];
void *opcode_args_2[256];
void executeOpcode();

// Macro to register opcode
// Args: (Opcode, Num cycles, handler function, opcode name, first argument, second argument)
#define REGISTER_OPCODE(o, c, h, n, a, b) {opcode_cycles[0x##o] = c; opcode_handlers[0x##o] = code_handler_##h; opcode_names[0x##o] = n; opcode_implemented[0x##o] = 1; opcode_args_1[0x##o] = a;opcode_args_2[0x##o] = b;}
//#define SET_ZN(value) cpu.F_Z = !(value); cpu.F_N = 
CPU cpu;
void initCPU();

