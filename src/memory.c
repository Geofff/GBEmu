#include "memory.h"
#include "cpu.h"
#include "gpu.h"

uint8_t readByte(uint16_t address){
    switch (address & 0xF000){
        // First 0x1000 is BIOS/ROM
        case 0x0000:
            if (address < BIOS_SIZE){
                return BIOS[address];
            }
        // ROM
        case 0x1000:
        case 0x2000:
        case 0x3000:
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000:
            return ROM[address];
        // Video RAM
        case 0x8000:
        case 0x9000:
            return VRAM[address & 0x1FFFF];
        // External RAM
        case 0xA000:
        case 0xB000:
            return ERAM[address & 0x1FFFF];
        // Working Wram
        case 0xC000:
        case 0xD000:
        case 0xE000:
            return WRAM[address & 0x1FFF];
        // Working RAM and DMA
        case 0xF000: {
            switch(address & 0xF00){
                // Graphics OAM
                case 0xE00:
                    if (address < 0xFEA0)
                        return OAM[address & 0xFF];
                    return 0;
                case 0xF00:
                    if (address >= 0xFF80){
                        return ZRAM[address & 0x7F];
                    }
                    return 0;
                default:
                    return WRAM[address & 0x1FFF];

            }
        }
    }
    return 0;
}

uint16_t readWord(uint16_t address){
    return (readByte(address) << 8) + readByte(address+1);
}

void writeByte(uint16_t address, uint8_t value){
        switch (address & 0xF000){
        // First 0x1000 is BIOS/ROM
        case 0x0000:
            if (address < BIOS_SIZE){
                BIOS[address] = value;
                return;
            }
        // ROM
        case 0x1000:
        case 0x2000:
        case 0x3000:
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000:
            ROM[address] = value;;
            return;
        // Video RAM
        case 0x8000:
        case 0x9000:
            VRAM[address & 0x1FFF] = value;
            printf("Wrote to VRAM: 0x%02X\n", value);
            if (address < 0x9800){
                updateTileSet(address, value);
            }
            return;
        // External RAM
        case 0xA000:
        case 0xB000:
            ERAM[address & 0x1FFF] = value;
            return;
        // Working Wram
        case 0xC000:
        case 0xD000:
        case 0xE000:
            WRAM[address & 0x1FFF] = value;
            return;
        // Working RAM and DMA
        case 0xF000: {
            switch(address & 0xF00){
                // Graphics OAM
                case 0xE00:
                    if (address < 0xFEA0)
                        OAM[address & 0xFF] = value;
                    return;
                case 0xF00:
                    if (address >= 0xFF80){
                        ZRAM[address & 0x7F] = value;
                    } else {
                        switch(address){
                            // LCD control;
                            case 0xFF40:
                                gpu.bgMode = (0b00000001 & value) ? 1 : 0;
                                gpu.bgMap = (0b00001000 & value) ? 1 : 0;
                                gpu.bgTile = (0b00010000 & value) ? 1 : 0;
                                gpu.switchLCD = (0b10000000 & value) ? 1 : 0;
                                break;
                            // Scroll Y
                            case 0xFF42:
                                gpu.scanY = value;
                                break;
                            // Scroll X
                            case 0xFF43:
                                gpu.scanX = value;
                                break;
                            
                            // Background pallete
                            case 0xFF47:
                                printf("Wrote to pallete\n");
                                break;


                        }
                    }
                    return;
                default:
                    WRAM[address & 0x1FFF] = value;
                    return;

            }
        }
    }
    assert(value == readByte(address));
}

void writeWord(uint16_t address, uint16_t value){
    writeByte(address, value&0x00FF);
    writeByte(address+1, value&0xFF00 >> 8);
}

void resetMemory(){
    memset(memory, 0, 0x10000);
}

void readFile(uint16_t offset, FILE *f){
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t val;
    for(int i = 0; i < size; i++){
        fread(&val, sizeof(uint8_t), 1, f);
        writeByte(offset+i, val);
    }
}

void pushByteToStack(uint8_t value){
    cpu.SP--;
    writeByte(cpu.SP, value);
}

void pushWordToStack(uint16_t value){
    cpu.SP--;
    writeByte(cpu.SP, value>>8);
    cpu.SP--;
    writeByte(cpu.SP, value&0xFF);
}

uint8_t readByteFromStack(){
    return readByte(cpu.SP++);
}

uint16_t readWordFromStack(){
    uint16_t val = 0;
    val += readByte(cpu.SP++);
    val += (readByte(cpu.SP++)<<8);
    return val;
}