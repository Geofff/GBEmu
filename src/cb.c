#include "cb.h"
#include "cpu.h"

void cb_handler_rlc(){
    uint8_t value = (*(uint8_t*)cb_args_1[cbcode] << 1) | (*(uint8_t*)cb_args_1[cbcode] >> 7);
    cpu.F_C = ((*(uint8_t*)cb_args_1[cbcode]) >> 7);
    cpu.F_Z = (value == 0);
    *(uint8_t*)cb_args_1[cbcode] = value;
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_rlc_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t value = (read << 1) | (read >> 7);
    cpu.F_C = (read >> 7);
    cpu.F_Z = (value == 0);
    writeByte(*(uint16_t*)cb_args_1[cbcode], value);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_rrc(){
    uint8_t value = (*(uint8_t*)cb_args_1[cbcode] >> 1) | (*(uint8_t*)cb_args_1[cbcode] << 7);
    cpu.F_C = ((*(uint8_t*)cb_args_1[cbcode]) &0b1);
    cpu.F_Z = (value == 0);
    *(uint8_t*)cb_args_1[cbcode] = value;
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_rrc_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t value = (read >> 1) | (read << 7);
    cpu.F_C = (read << 7);
    cpu.F_Z = (value == 0);
    writeByte(*(uint16_t*)cb_args_1[cbcode], value);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_rl(){
    uint8_t value = (*(uint8_t*)cb_args_1[cbcode] << 1) | (cpu.F_C);
    cpu.F_C = ((*(uint8_t*)cb_args_1[cbcode]) >> 7);
    cpu.F_Z = (value == 0);
    *(uint8_t*)cb_args_1[cbcode] = value;
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_rl_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t value = (read << 1) | (cpu.F_C);
    cpu.F_C = (read >> 7);
    cpu.F_Z = (value == 0);
    writeByte(*(uint16_t*)cb_args_1[cbcode], value);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_rr(){
    uint8_t value = (*(uint8_t*)cb_args_1[cbcode] >> 1) | (cpu.F_C << 7);
    cpu.F_C = ((*(uint8_t*)cb_args_1[cbcode]) &0b1);
    cpu.F_Z = (value == 0);
    *(uint8_t*)cb_args_1[cbcode] = value;
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_rr_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t value = (read >> 1) | (cpu.F_C << 7);
    cpu.F_C = (read << 7);
    cpu.F_Z = (value == 0);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_sla(){
    uint8_t value = (*(uint8_t*)cb_args_1[cbcode]) << 1;
    cpu.F_C = (*(uint8_t*)cb_args_1[cbcode]) >> 7;
    *(uint8_t*)cb_args_1[cbcode] = value;
    cpu.F_Z = (value == 0);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_sla_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t value = read << 1;
    cpu.F_C = read >> 7;
    writeByte(*(uint16_t*)cb_args_1[cbcode], value);
    cpu.F_Z = (value == 0);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_sra(){
    uint8_t value = ((*(uint8_t*)cb_args_1[cbcode]) >> 1) | ((*(uint8_t*)cb_args_1[cbcode]) &0b10000000);
    cpu.F_C = (*(uint8_t*)cb_args_1[cbcode])&0b1;
    *(uint8_t*)cb_args_1[cbcode] = value;
    cpu.F_Z = (value == 0);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_sra_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t value = read >> 1 | (read&0b10000000);
    cpu.F_C = read &0b1;
    writeByte(*(uint16_t*)cb_args_1[cbcode], value);
    cpu.F_Z = (value == 0);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_srl(){
    uint8_t value = (*(uint8_t*)cb_args_1[cbcode]) >> 1;
    cpu.F_C = (*(uint8_t*)cb_args_1[cbcode])&0b1;
    *(uint8_t*)cb_args_1[cbcode] = value;
    cpu.F_Z = (value == 0);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_srl_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t value = read >> 1;
    cpu.F_C = read &0b1;
    writeByte(*(uint16_t*)cb_args_1[cbcode], value);
    cpu.F_Z = (value == 0);
    cpu.F_H = 0;
    cpu.F_N = 0;
}

void cb_handler_swap(){
    uint8_t read = (*(uint8_t*)cb_args_1[cbcode]);
    uint8_t value = (read >> 4) | ((read<<4)&0xF0);
    cpu.F_Z = (value == 0);
    cpu.F_C = 0;
    cpu.F_H = 0;
    cpu.F_N = 0;
    (*(uint8_t*)cb_args_1[cbcode]) = value;
}

void cb_handler_swap_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t value = (read >> 4) | ((read<<4)&0xF0);
    cpu.F_Z = (value == 0);
    cpu.F_C = 0;
    cpu.F_H = 0;
    cpu.F_N = 0;
    writeByte(*(uint16_t*)cb_args_1[cbcode], value);
}

void cb_handler_bit(){
    uint8_t read = (*(uint8_t*)cb_args_1[cbcode]);
    uint8_t bit = (uint8_t)cb_args_2[cbcode];
    if ((1 << bit)&read){
        cpu.F_Z = 0;
    } else {
        cpu.F_Z = 1;
    }
    cpu.F_H = 1;
    cpu.F_N = 0;
}

void cb_handler_bit_indirect(){
    uint8_t read = readByte(*(uint16_t*)cb_args_1[cbcode]);
    uint8_t bit = (uint8_t)cb_args_2[cbcode];
    if ((1 << bit)&read){
        cpu.F_Z = 0;
    } else {
        cpu.F_Z = 1;
    }
    cpu.F_H = 1;
    cpu.F_N = 0;
}

void setupCB(){
    REGISTER_CB( 0, 8, rlc, "RLC B", &cpu.B, NULL);
    REGISTER_CB( 1, 8, rlc, "RLC C", &cpu.C, NULL);
    REGISTER_CB( 2, 8, rlc, "RLC D", &cpu.D, NULL);
    REGISTER_CB( 3, 8, rlc, "RLC E", &cpu.E, NULL);
    REGISTER_CB( 4, 8, rlc, "RLC H", &cpu.H, NULL);
    REGISTER_CB( 5, 8, rlc, "RLC L", &cpu.L, NULL);
    REGISTER_CB( 6, 16, rlc_indirect, "RLC (HL)", &cpu.HL, NULL);
    REGISTER_CB( 7, 8, rrc, "RRC A", &cpu.A, NULL);
    REGISTER_CB( 8, 8, rrc, "RRC B", &cpu.B, NULL);
    REGISTER_CB( 9, 8, rrc, "RRC C", &cpu.C, NULL);
    REGISTER_CB( A, 8, rrc, "RRC D", &cpu.D, NULL);
    REGISTER_CB( B, 8, rrc, "RRC E", &cpu.E, NULL);
    REGISTER_CB( C, 8, rrc, "RRC H", &cpu.H, NULL);
    REGISTER_CB( D, 8, rrc, "RRC L", &cpu.L, NULL);
    REGISTER_CB( E, 16, rrc_indirect, "RRC (HL)", &cpu.HL, NULL);
    REGISTER_CB( F, 8, rrc, "RRC A", &cpu.A, NULL);
    REGISTER_CB( 10, 8, rl, "RL B", &cpu.B, NULL);
    REGISTER_CB( 11, 8, rl, "RL C", &cpu.C, NULL);
    REGISTER_CB( 12, 8, rl, "RL D", &cpu.D, NULL);
    REGISTER_CB( 13, 8, rl, "RL E", &cpu.E, NULL);
    REGISTER_CB( 14, 8, rl, "RL H", &cpu.H, NULL);
    REGISTER_CB( 15, 8, rl, "RL L", &cpu.L, NULL);
    REGISTER_CB( 16, 16, rl_indirect, "RL (HL)", &cpu.HL, NULL);
    REGISTER_CB( 17, 8, rr, "RR A", &cpu.A, NULL);
    REGISTER_CB( 18, 8, rr, "RR B", &cpu.B, NULL);
    REGISTER_CB( 19, 8, rr, "RR C", &cpu.C, NULL);
    REGISTER_CB( 1A, 8, rr, "RR D", &cpu.D, NULL);
    REGISTER_CB( 1B, 8, rr, "RR E", &cpu.E, NULL);
    REGISTER_CB( 1C, 8, rr, "RR H", &cpu.H, NULL);
    REGISTER_CB( 1D, 8, rr, "RR L", &cpu.L, NULL);
    REGISTER_CB( 1E, 16, rr_indirect, "RR (HL)", &cpu.HL, NULL);
    REGISTER_CB( 1F, 8, rr, "RR A", &cpu.A, NULL);
    REGISTER_CB( 20, 8, sla, "SLA B", &cpu.B, NULL);
    REGISTER_CB( 21, 8, sla, "SLA C", &cpu.C, NULL);
    REGISTER_CB( 22, 8, sla, "SLA D", &cpu.D, NULL);
    REGISTER_CB( 23, 8, sla, "SLA E", &cpu.E, NULL);
    REGISTER_CB( 24, 8, sla, "SLA H", &cpu.H, NULL);
    REGISTER_CB( 25, 8, sla, "SLA L", &cpu.L, NULL);
    REGISTER_CB( 26, 16, sla_indirect, "SLA (HL)", &cpu.HL, NULL);
    REGISTER_CB( 27, 8, sra, "SRA A", &cpu.A, NULL);
    REGISTER_CB( 28, 8, sra, "SRA B", &cpu.B, NULL);
    REGISTER_CB( 29, 8, sra, "SRA C", &cpu.C, NULL);
    REGISTER_CB( 2A, 8, sra, "SRA D", &cpu.D, NULL);
    REGISTER_CB( 2B, 8, sra, "SRA E", &cpu.E, NULL);
    REGISTER_CB( 2C, 8, sra, "SRA H", &cpu.H, NULL);
    REGISTER_CB( 2D, 8, sra, "SRA L", &cpu.L, NULL);
    REGISTER_CB( 2E, 16, sra_indirect, "SRA (HL)", &cpu.HL, NULL);
    REGISTER_CB( 2F, 8, sra, "SRA A", &cpu.A, NULL);
    REGISTER_CB( 30, 8, swap, "SWAP B", &cpu.B, NULL);
    REGISTER_CB( 31, 8, swap, "SWAP C", &cpu.C, NULL);
    REGISTER_CB( 32, 8, swap, "SWAP D", &cpu.D, NULL);
    REGISTER_CB( 33, 8, swap, "SWAP E", &cpu.E, NULL);
    REGISTER_CB( 34, 8, swap, "SWAP H", &cpu.H, NULL);
    REGISTER_CB( 35, 8, swap, "SWAP L", &cpu.L, NULL);
    REGISTER_CB( 36, 16, swap_indirect, "SWAP (HL)", &cpu.HL, NULL);
    REGISTER_CB( 37, 8, sra, "SRA A", &cpu.A, NULL);
    REGISTER_CB( 38, 8, srl, "SRL B", &cpu.B, NULL);
    REGISTER_CB( 39, 8, srl, "SRL C", &cpu.C, NULL);
    REGISTER_CB( 3A, 8, srl, "SRL D", &cpu.D, NULL);
    REGISTER_CB( 3B, 8, srl, "SRL E", &cpu.E, NULL);
    REGISTER_CB( 3C, 8, srl, "SRL H", &cpu.H, NULL);
    REGISTER_CB( 3D, 8, srl, "SRL L", &cpu.L, NULL);
    REGISTER_CB( 3E, 16, srl_indirect, "SRL (HL)", &cpu.HL, NULL);
    REGISTER_CB( 3F, 8, srl, "SRL A", &cpu.A, NULL);
    REGISTER_CB( 40, 8, bit, "BIT 0,B", &cpu.B, (void*)0);
    REGISTER_CB( 41, 8, bit, "BIT 0,C", &cpu.C, (void*)0);
    REGISTER_CB( 42, 8, bit, "BIT 0,D", &cpu.D, (void*)0);
    REGISTER_CB( 43, 8, bit, "BIT 0,E", &cpu.E, (void*)0);
    REGISTER_CB( 44, 8, bit, "BIT 0,H", &cpu.H, (void*)0);
    REGISTER_CB( 45, 8, bit, "BIT 0,L", &cpu.L, (void*)0);
    REGISTER_CB( 46, 16, bit_indirect, "BIT 0,(HL)", &cpu.HL, (void*)0);
    REGISTER_CB( 47, 8, bit, "BIT 0,A", &cpu.A, (void*)0);
    REGISTER_CB( 48, 8, bit, "BIT 1,B", &cpu.B, (void*)0);
    REGISTER_CB( 49, 8, bit, "BIT 1,C", &cpu.C, (void*)0);
    REGISTER_CB( 4A, 8, bit, "BIT 1,D", &cpu.D, (void*)0);
    REGISTER_CB( 4B, 8, bit, "BIT 1,E", &cpu.E, (void*)0);
    REGISTER_CB( 4C, 8, bit, "BIT 1,H", &cpu.H, (void*)0);
    REGISTER_CB( 4D, 8, bit, "BIT 1,L", &cpu.L, (void*)0);
    REGISTER_CB( 4E, 16, bit_indirect, "BIT 1,(HL)", &cpu.HL, (void*)0);
    REGISTER_CB( 4F, 8, bit, "BIT 1,A", &cpu.A, (void*)0);
    REGISTER_CB( 50, 8, bit, "BIT 2,B", &cpu.B, (void*)0);
    REGISTER_CB( 51, 8, bit, "BIT 2,C", &cpu.C, (void*)0);
    REGISTER_CB( 52, 8, bit, "BIT 2,D", &cpu.D, (void*)0);
    REGISTER_CB( 53, 8, bit, "BIT 2,E", &cpu.E, (void*)0);
    REGISTER_CB( 54, 8, bit, "BIT 2,H", &cpu.H, (void*)0);
    REGISTER_CB( 55, 8, bit, "BIT 2,L", &cpu.L, (void*)0);
    REGISTER_CB( 56, 16, bit_indirect, "BIT 0,(HL)", &cpu.HL, (void*)0);
    REGISTER_CB( 57, 8, bit, "BIT 2,A", &cpu.A, (void*)0);
    REGISTER_CB( 58, 8, bit, "BIT 3,B", &cpu.B, (void*)0);
    REGISTER_CB( 59, 8, bit, "BIT 3,C", &cpu.C, (void*)0);
    REGISTER_CB( 5A, 8, bit, "BIT 3,D", &cpu.D, (void*)0);
    REGISTER_CB( 5B, 8, bit, "BIT 3,E", &cpu.E, (void*)0);
    REGISTER_CB( 5C, 8, bit, "BIT 3,H", &cpu.H, (void*)0);
    REGISTER_CB( 5D, 8, bit, "BIT 3,L", &cpu.L, (void*)0);
    REGISTER_CB( 5E, 16, bit_indirect, "BIT 1,(HL)", &cpu.HL, (void*)0);
    REGISTER_CB( 5F, 8, bit, "BIT 3,A", &cpu.A, (void*)0);
    REGISTER_CB( 60, 8, bit, "BIT 4,B", &cpu.B, (void*)0);
    REGISTER_CB( 61, 8, bit, "BIT 4,C", &cpu.C, (void*)0);
    REGISTER_CB( 62, 8, bit, "BIT 4,D", &cpu.D, (void*)0);
    REGISTER_CB( 63, 8, bit, "BIT 4,E", &cpu.E, (void*)0);
    REGISTER_CB( 64, 8, bit, "BIT 4,H", &cpu.H, (void*)0);
    REGISTER_CB( 65, 8, bit, "BIT 4,L", &cpu.L, (void*)0);
    REGISTER_CB( 66, 16, bit_indirect, "BIT 0,(HL)", &cpu.HL, (void*)0);
    REGISTER_CB( 67, 8, bit, "BIT 4,A", &cpu.A, (void*)0);
    REGISTER_CB( 68, 8, bit, "BIT 5,B", &cpu.B, (void*)0);
    REGISTER_CB( 69, 8, bit, "BIT 5,C", &cpu.C, (void*)0);
    REGISTER_CB( 6A, 8, bit, "BIT 5,D", &cpu.D, (void*)0);
    REGISTER_CB( 6B, 8, bit, "BIT 5,E", &cpu.E, (void*)0);
    REGISTER_CB( 6C, 8, bit, "BIT 5,H", &cpu.H, (void*)0);
    REGISTER_CB( 6D, 8, bit, "BIT 5,L", &cpu.L, (void*)0);
    REGISTER_CB( 6E, 16, bit_indirect, "BIT 1,(HL)", &cpu.HL, (void*)0);
    REGISTER_CB( 6F, 8, bit, "BIT 5,A", &cpu.A, (void*)0);
    REGISTER_CB( 70, 8, bit, "BIT 6,B", &cpu.B, (void*)0);
    REGISTER_CB( 71, 8, bit, "BIT 6,C", &cpu.C, (void*)0);
    REGISTER_CB( 72, 8, bit, "BIT 6,D", &cpu.D, (void*)0);
    REGISTER_CB( 73, 8, bit, "BIT 6,E", &cpu.E, (void*)0);
    REGISTER_CB( 74, 8, bit, "BIT 6,H", &cpu.H, (void*)0);
    REGISTER_CB( 75, 8, bit, "BIT 6,L", &cpu.L, (void*)0);
    REGISTER_CB( 76, 16, bit_indirect, "BIT 0,(HL)", &cpu.HL, (void*)0);
    REGISTER_CB( 77, 8, bit, "BIT 6,A", &cpu.A, (void*)0);
    REGISTER_CB( 78, 8, bit, "BIT 7,B", &cpu.B, (void*)0);
    REGISTER_CB( 79, 8, bit, "BIT 7,C", &cpu.C, (void*)0);
    REGISTER_CB( 7A, 8, bit, "BIT 7,D", &cpu.D, (void*)0);
    REGISTER_CB( 7B, 8, bit, "BIT 7,E", &cpu.E, (void*)0);
    REGISTER_CB( 7C, 8, bit, "BIT 7,H", &cpu.H, (void*)0);
    REGISTER_CB( 7D, 8, bit, "BIT 7,L", &cpu.L, (void*)0);
    REGISTER_CB( 7E, 16, bit_indirect, "BIT 1,(HL)", &cpu.HL, (void*)0);
    REGISTER_CB( 7F, 8, bit, "BIT 7,A", &cpu.A, (void*)0);
}

void executeCB(uint8_t code){
    cbcode = code;
    cpu.ticks += cb_cycles[code];
    if (cb_implemented[code] == 0){
        printf("CB code 0x%X at 0x%X is not implemented\n", code, cpu.PC);
        cpu.ticks+=1000;
        cpu.running = 0;
        return;
    }
    
    cb_handlers[code]();
}