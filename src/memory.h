#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define BIOS_SIZE 0x100
#define ROM_SIZE 0x8000
#define WRAM_SIZE 0x2000
#define ZRAM_SIZE 0x80
#define ERAM_SIZE 262144
#define VRAM_SIZE 10000
#define OAM_SIZE 1000


uint8_t memory[0x10000];

uint8_t BIOS[BIOS_SIZE];
uint8_t ROM[ROM_SIZE];
uint8_t WRAM[WRAM_SIZE];
uint8_t ZRAM[ZRAM_SIZE];
uint8_t ERAM[ERAM_SIZE];
uint8_t VRAM[VRAM_SIZE];
uint8_t OAM[OAM_SIZE];

uint8_t readByte(uint16_t address);
uint16_t readWord(uint16_t address);
void writeByte(uint16_t address, uint8_t value);
void pushByteToStack(uint8_t value);
uint8_t readByteFromStack();
void pushWordToStack(uint16_t value);
uint16_t readWordFromStack();
void writeWord(uint16_t address, uint16_t value);
void resetMemory();
void readFile(uint16_t offset, FILE *f);
