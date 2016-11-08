#include "cpu.h"

// No operation
void code_handler_nop(){}

// Load byte
void code_handler_ld_b(){
    *(uint8_t*)opcode_args_1[cpu.PC] = *(uint8_t*)opcode_args_2[cpu.PC];
}

// Load word
void code_handler_ld_w(){
    *(uint16_t*)opcode_args_1[cpu.PC] = *(uint16_t*)opcode_args_2[cpu.PC];
}

// Increment byte
void code_handler_inc_b(){
    *(uint8_t*)opcode_args_1[cpu.PC]++;
    // TODO Flags? ZOH-
}

// Increment word
void code_handler_inc_w(){
    *(uint16_t*)opcode_args_1[cpu.PC]++;
}

// Decrement byte
void code_handler_dec_b(){
    *(uint8_t*)opcode_args_1[cpu.PC]--;
    // TODO Flags? ZOH-
}

// Decrement word
void code_handler_dec_w(){
    *(uint16_t*)opcode_args_1[cpu.PC]--;
}

// Add byte
void code_handler_add_b(){
    *(uint8_t*)opcode_args_1[cpu.PC] += *(uint8_t*)opcode_args_2[cpu.PC];
    // TODO FLAGS
    // Z0HC
}

// Add byte with carry
void code_handler_adc_b(){
    *(uint8_t*)opcode_args_1[cpu.PC] += *(uint8_t*)opcode_args_2[cpu.PC] + cpu.F_C;
    // TODO FLAGS
    // Z0HC
}

// Sub byte
void code_handler_sub_b(){
    *(uint8_t*)opcode_args_1[cpu.PC] -= *(uint8_t*)opcode_args_2[cpu.PC];
    // TODO FLAGS
    // Z1HC
}

// Sub byte with carry
void code_handler_sbc_b(){
    *(uint8_t*)opcode_args_1[cpu.PC] -= *(uint8_t*)opcode_args_2[cpu.PC] + cpu.F_C;
    // TODO FLAGS
    // Z1HC
}

// bitwise AND
void code_handler_and_b(){
    cpu.A &= *(uint8_t*)opcode_args_2[cpu.PC];
    // TODO FLAGS
    // Z010
}

// bitwise XOR
void code_handler_xor_b(){
    cpu.A ^= *(uint8_t*)opcode_args_2[cpu.PC];
    // TODO FLAGS
    // Z000
}

// bitwise OR
void code_handler_or_b(){
    cpu.A |= *(uint8_t*)opcode_args_2[cpu.PC];
    // TODO FLAGS
    // Z000
}

// bitwise CP
void code_handler_cp_b(){
    // TODO FLAGS
    // Z1HC
}



void initCPU(){
    REGISTER_OPCODE( 0, 4, nop, "nop", 0, 0);
    REGISTER_OPCODE( 1, 12, ld_w, "LD BC,d16", &cpu.BC, &cpu.d16);
    REGISTER_OPCODE(11, 12, ld_w, "LD DE,d16", &cpu.DE, &cpu.d16);
    REGISTER_OPCODE(21, 12, ld_w, "LD HL,d16", &cpu.HL, &cpu.d16);
    REGISTER_OPCODE(31, 12, ld_w, "LD SP,d16", &cpu.SP, &cpu.d16);

    REGISTER_OPCODE( 3, 8, inc_w, "INC BC", &cpu.BC, 0);
    REGISTER_OPCODE(13, 8, inc_w, "INC DE", &cpu.DE, 0);
    REGISTER_OPCODE(23, 8, inc_w, "INC HL", &cpu.HL, 0);
    REGISTER_OPCODE(33, 8, inc_w, "INC SP", &cpu.SP, 0);
    REGISTER_OPCODE( 4, 4, inc_b, "INC B", &cpu.B, 0);
    REGISTER_OPCODE(14, 4, inc_b, "INC D", &cpu.D, 0);
    REGISTER_OPCODE(24, 4, inc_b, "INC H", &cpu.H, 0);
    REGISTER_OPCODE(34, 12, inc_b, "INC (HL)", &cpu.HL_VAL, 0);
    REGISTER_OPCODE( 5, 4, dec_b, "DEC B", &cpu.B, 0);
    REGISTER_OPCODE(15, 4, dec_b, "DEC D", &cpu.D, 0);
    REGISTER_OPCODE(25, 4, dec_b, "DEC H", &cpu.H, 0);
    REGISTER_OPCODE(35, 12, dec_b, "DEC (HL)", &cpu.HL_VAL, 0);

    REGISTER_OPCODE( B, 8, dec_w, "DEC BC", &cpu.BC, 0);
    REGISTER_OPCODE(1B, 8, dec_w, "DEC DE", &cpu.DE, 0);
    REGISTER_OPCODE(2B, 8, dec_w, "DEC HL", &cpu.HL, 0);
    REGISTER_OPCODE(3B, 8, dec_w, "DEC SP", &cpu.SP, 0);
    REGISTER_OPCODE( C, 4, inc_b, "INC C", &cpu.C, 0);
    REGISTER_OPCODE(1C, 4, inc_b, "INC E", &cpu.E, 0);
    REGISTER_OPCODE(2C, 4, inc_b, "INC L", &cpu.L, 0);
    REGISTER_OPCODE(3C, 4, inc_b, "INC A", &cpu.A, 0);
    REGISTER_OPCODE( D, 4, dec_b, "DEC C", &cpu.C, 0);
    REGISTER_OPCODE(1D, 4, dec_b, "DEC E", &cpu.E, 0);
    REGISTER_OPCODE(2D, 4, dec_b, "DEC L", &cpu.L, 0);
    REGISTER_OPCODE(3D, 4, dec_b, "DEC A", &cpu.A, 0);



    REGISTER_OPCODE(40, 4, ld_b, "LD B,B", &cpu.B, &cpu.B);
    REGISTER_OPCODE(41, 4, ld_b, "LD B,C", &cpu.B, &cpu.C);
    REGISTER_OPCODE(42, 4, ld_b, "LD B,D", &cpu.B, &cpu.D);
    REGISTER_OPCODE(43, 4, ld_b, "LD B,E", &cpu.B, &cpu.E);
    REGISTER_OPCODE(44, 4, ld_b, "LD B,H", &cpu.B, &cpu.H);
    REGISTER_OPCODE(45, 4, ld_b, "LD B,L", &cpu.B, &cpu.L);
    REGISTER_OPCODE(46, 8, ld_b, "LD B,(HL)", &cpu.B, cpu.HL_VAL);
    REGISTER_OPCODE(47, 4, ld_b, "LD B,A", &cpu.B, &cpu.A);

    REGISTER_OPCODE(48, 4, ld_b, "LD C,B", &cpu.C, &cpu.B);
    REGISTER_OPCODE(49, 4, ld_b, "LD C,C", &cpu.C, &cpu.C);
    REGISTER_OPCODE(4A, 4, ld_b, "LD C,D", &cpu.C, &cpu.D);
    REGISTER_OPCODE(4B, 4, ld_b, "LD C,E", &cpu.C, &cpu.E);
    REGISTER_OPCODE(4C, 4, ld_b, "LD C,H", &cpu.C, &cpu.H);
    REGISTER_OPCODE(4D, 4, ld_b, "LD C,L", &cpu.C, &cpu.L);
    REGISTER_OPCODE(4E, 8, ld_b, "LD C,(HL)", &cpu.C, cpu.HL_VAL);
    REGISTER_OPCODE(4F, 4, ld_b, "LD C,A", &cpu.C, &cpu.A);

    REGISTER_OPCODE(50, 4, ld_b, "LD D,B", &cpu.D, &cpu.B);
    REGISTER_OPCODE(51, 4, ld_b, "LD D,C", &cpu.D, &cpu.C);
    REGISTER_OPCODE(52, 4, ld_b, "LD D,D", &cpu.D, &cpu.D);
    REGISTER_OPCODE(53, 4, ld_b, "LD D,E", &cpu.D, &cpu.E);
    REGISTER_OPCODE(54, 4, ld_b, "LD D,H", &cpu.D, &cpu.H);
    REGISTER_OPCODE(55, 4, ld_b, "LD D,L", &cpu.D, &cpu.L);
    REGISTER_OPCODE(56, 8, ld_b, "LD D,(HL)", &cpu.D, cpu.HL_VAL);
    REGISTER_OPCODE(57, 4, ld_b, "LD D,A", &cpu.D, &cpu.A);

    REGISTER_OPCODE(58, 4, ld_b, "LD E,B", &cpu.E, &cpu.B);
    REGISTER_OPCODE(59, 4, ld_b, "LD E,C", &cpu.E, &cpu.C);
    REGISTER_OPCODE(5A, 4, ld_b, "LD E,D", &cpu.E, &cpu.D);
    REGISTER_OPCODE(5B, 4, ld_b, "LD E,E", &cpu.E, &cpu.E);
    REGISTER_OPCODE(5C, 4, ld_b, "LD E,H", &cpu.E, &cpu.H);
    REGISTER_OPCODE(5D, 4, ld_b, "LD E,L", &cpu.E, &cpu.L);
    REGISTER_OPCODE(5E, 8, ld_b, "LD E,(HL)", &cpu.E, cpu.HL_VAL);
    REGISTER_OPCODE(5F, 4, ld_b, "LD E,A", &cpu.E, &cpu.A);

    REGISTER_OPCODE(60, 4, ld_b, "LD H,B", &cpu.H, &cpu.B);
    REGISTER_OPCODE(61, 4, ld_b, "LD H,C", &cpu.H, &cpu.C);
    REGISTER_OPCODE(62, 4, ld_b, "LD H,D", &cpu.H, &cpu.D);
    REGISTER_OPCODE(63, 4, ld_b, "LD H,E", &cpu.H, &cpu.E);
    REGISTER_OPCODE(64, 4, ld_b, "LD H,H", &cpu.H, &cpu.H);
    REGISTER_OPCODE(65, 4, ld_b, "LD H,L", &cpu.H, &cpu.L);
    REGISTER_OPCODE(66, 8, ld_b, "LD H,(HL)", &cpu.H, cpu.HL_VAL);
    REGISTER_OPCODE(67, 4, ld_b, "LD H,A", &cpu.H, &cpu.A);

    REGISTER_OPCODE(68, 4, ld_b, "LD L,B", &cpu.L, &cpu.B);
    REGISTER_OPCODE(69, 4, ld_b, "LD L,C", &cpu.L, &cpu.C);
    REGISTER_OPCODE(6A, 4, ld_b, "LD L,D", &cpu.L, &cpu.D);
    REGISTER_OPCODE(6B, 4, ld_b, "LD L,E", &cpu.L, &cpu.E);
    REGISTER_OPCODE(6C, 4, ld_b, "LD L,H", &cpu.L, &cpu.H);
    REGISTER_OPCODE(6D, 4, ld_b, "LD L,L", &cpu.L, &cpu.L);
    REGISTER_OPCODE(6E, 8, ld_b, "LD L,(HL)", &cpu.L, cpu.HL_VAL);
    REGISTER_OPCODE(6F, 4, ld_b, "LD L,A", &cpu.L, &cpu.A);

    REGISTER_OPCODE(70, 8, ld_b, "LD (HL),B", cpu.HL_VAL, &cpu.B);
    REGISTER_OPCODE(71, 8, ld_b, "LD (HL),C", cpu.HL_VAL, &cpu.C);
    REGISTER_OPCODE(72, 8, ld_b, "LD (HL),D", cpu.HL_VAL, &cpu.D);
    REGISTER_OPCODE(73, 8, ld_b, "LD (HL),E", cpu.HL_VAL, &cpu.E);
    REGISTER_OPCODE(74, 8, ld_b, "LD (HL),H", cpu.HL_VAL, &cpu.H);
    REGISTER_OPCODE(75, 8, ld_b, "LD (HL),L", cpu.HL_VAL, &cpu.L);
    // 0x76 is HALT
    REGISTER_OPCODE(77, 8, ld_b, "LD (HL),A", cpu.HL_VAL, &cpu.A);

    REGISTER_OPCODE(78, 4, ld_b, "LD A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(79, 4, ld_b, "LD A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(7A, 4, ld_b, "LD A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(7B, 4, ld_b, "LD A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(7C, 4, ld_b, "LD A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(7D, 4, ld_b, "LD A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(7E, 8, ld_b, "LD A,(HL)", &cpu.A, cpu.HL_VAL);
    REGISTER_OPCODE(7F, 4, ld_b, "LD A,A", &cpu.A, &cpu.A);

    REGISTER_OPCODE(80, 4, add_b, "ADD A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(81, 4, add_b, "ADD A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(82, 4, add_b, "ADD A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(83, 4, add_b, "ADD A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(84, 4, add_b, "ADD A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(85, 4, add_b, "ADD A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(86, 8, add_b, "ADD A,(HL)", &cpu.A, &cpu.HL_VAL);
    REGISTER_OPCODE(87, 4, add_b, "ADD A,A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(88, 4, adc_b, "ADC A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(89, 4, adc_b, "ADC A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(8A, 4, adc_b, "ADC A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(8B, 4, adc_b, "ADC A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(8C, 4, adc_b, "ADC A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(8D, 4, adc_b, "ADC A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(8E, 8, adc_b, "ADC A,(HL)", &cpu.A, &cpu.HL_VAL);
    REGISTER_OPCODE(8F, 4, adc_b, "ADC A,A", &cpu.A, &cpu.A);

    REGISTER_OPCODE(90, 4, sub_b, "SUB A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(91, 4, sub_b, "SUB A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(92, 4, sub_b, "SUB A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(93, 4, sub_b, "SUB A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(94, 4, sub_b, "SUB A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(95, 4, sub_b, "SUB A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(96, 8, sub_b, "SUB A,(HL)", &cpu.A, &cpu.HL_VAL);
    REGISTER_OPCODE(97, 4, sub_b, "SUB A,A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(98, 4, sbc_b, "SBC A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(99, 4, sbc_b, "SBC A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(9A, 4, sbc_b, "SBC A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(9B, 4, sbc_b, "SBC A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(9C, 4, sbc_b, "SBC A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(9D, 4, sbc_b, "SBC A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(9E, 8, sbc_b, "SBC A,(HL)", &cpu.A, &cpu.HL_VAL);
    REGISTER_OPCODE(9F, 4, sbc_b, "SBC A,A", &cpu.A, &cpu.A);

    REGISTER_OPCODE(A0, 4, and_b, "AND B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(A1, 4, and_b, "AND C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(A2, 4, and_b, "AND D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(A3, 4, and_b, "AND E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(A4, 4, and_b, "AND H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(A5, 4, and_b, "AND L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(A6, 8, and_b, "AND (HL)", &cpu.A, &cpu.HL_VAL);
    REGISTER_OPCODE(A7, 4, and_b, "AND A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(A8, 4, xor_b, "XOR B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(A9, 4, xor_b, "XOR C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(AA, 4, xor_b, "XOR D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(AB, 4, xor_b, "XOR E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(AC, 4, xor_b, "XOR H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(AD, 4, xor_b, "XOR L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(AE, 8, xor_b, "XOR (HL)", &cpu.A, &cpu.HL_VAL);
    REGISTER_OPCODE(AF, 4, xor_b, "XOR A", &cpu.A, &cpu.A);

    REGISTER_OPCODE(B0, 4, or_b, "OR B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(B1, 4, or_b, "OR C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(B2, 4, or_b, "OR D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(B3, 4, or_b, "OR E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(B4, 4, or_b, "OR H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(B5, 4, or_b, "OR L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(B6, 8, or_b, "OR (HL)", &cpu.A, &cpu.HL_VAL);
    REGISTER_OPCODE(B7, 4, or_b, "OR A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(B8, 4, cp_b, "CP B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(B9, 4, cp_b, "CP C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(BA, 4, cp_b, "CP D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(BB, 4, cp_b, "CP E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(BC, 4, cp_b, "CP H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(BD, 4, cp_b, "CP L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(BE, 8, cp_b, "CP (HL)", &cpu.A, &cpu.HL_VAL);
    REGISTER_OPCODE(BF, 4, cp_b, "CP A", &cpu.A, &cpu.A);
}

void executeOpcode(){
    if (opcode_implemented[cpu.PC] == 0){
        printf("Opcode 0x%X is not implemented\n", cpu.PC);
        return;
    }
    opcode_handlers[cpu.PC]();
    printf("Did %s for %d cycles\n", opcode_names[cpu.PC], opcode_cycles[cpu.PC]);
}