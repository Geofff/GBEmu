#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define RAM_SIZE 0x600
#define STACK_SIZE 0x1ff
#define S_ROM_SIZE 16384
#define E_WORK_RAM_SIZE 262144
#define I_WORK_RAM_SIZE 32768


uint8_t CART[0x8000];
uint8_t VRAM[0x2000];
uint8_t SRAM[0x2000];
uint8_t IRAM[0x2000];


uint8_t readByte(uint16_t address);
uint16_t readWord(uint16_t address);
void writeByte(uint16_t address, uint8_t value);
void writeWord(uint16_t address, uint16_t value);
void resetMemory();
