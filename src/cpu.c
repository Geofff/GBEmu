#include "cpu.h"

// No operation
void code_handler_nop(){}

// Load byte
void code_handler_ld_b(){
    printf("opcode is 0x%02X : %s : HL is 0x%02X\n", opcode, opcode_names[opcode], cpu.HL);
    *(uint8_t*)opcode_args_1[opcode] = *(uint8_t*)opcode_args_2[opcode];
}

// Load word
void code_handler_ld_w(){
    *(uint16_t*)opcode_args_1[opcode] = *(uint16_t*)opcode_args_2[opcode];
}

// Load word indirectly
void code_handler_ld_w_indirect_read(){
    *(uint16_t*)opcode_args_1[opcode] = readWord(*(uint16_t*)opcode_args_2[opcode]);
}

// Load word indirectly
void code_handler_ld_b_indirect_read(){
    *(uint8_t*)opcode_args_1[opcode] = readByte(0xFF00+*(uint8_t*)opcode_args_2[opcode]);
}

void code_handler_ld_b_indirect_write(){
    writeByte(0xFF00+*(uint8_t*)opcode_args_2[opcode], *(uint8_t*)opcode_args_1[opcode]);
}

void code_handler_ldi_w_indirect(){
    writeByte((*(uint16_t*)opcode_args_1[opcode])++, cpu.A);
}

void code_handler_ldd_w_indirect(){
    writeByte((*(uint16_t*)opcode_args_1[opcode])--, cpu.A);
}

void code_handler_lda_hp_indirect(){
    cpu.A = readByte((*(uint16_t*)opcode_args_1[opcode])++);
}

void code_handler_lda_hn_indirect(){
    cpu.A = readByte((*(uint16_t*)opcode_args_1[opcode])--);
}


// Load word indirectly
void code_handler_ld_w_indirect(){
    writeWord(*(uint16_t*)opcode_args_1[opcode], *(uint16_t*)opcode_args_2[opcode]);
}

// Load word indirectly
void code_handler_ldh_b_indirect_r(){
    *(uint16_t*)opcode_args_2[opcode] = readWord(0xFF00+(*(uint8_t*)opcode_args_1[opcode]));
}

void code_handler_ldh_b_indirect_w(){
    writeByte(0xFF00+(*(uint8_t*)opcode_args_1[opcode]), *(uint16_t*)opcode_args_2[opcode]);
}

void code_handler_call(){
    pushWordToStack(cpu.PC+3);
    cpu.PC = *(uint16_t*)opcode_args_1[opcode];
}

void code_handler_call_cond(){
    if (*(uint8_t*)opcode_args_2[opcode]){
        pushWordToStack(cpu.PC+3);
        cpu.PC = *(uint16_t*)opcode_args_1[opcode];
    } else {
        cpu.PC++;
    }
}

void code_handler_call_cond_inv(){
    if (!(*(uint8_t*)opcode_args_2[opcode])){
        pushWordToStack(cpu.PC+3);
        cpu.PC = *(uint16_t*)opcode_args_1[opcode];
    } else {
        cpu.PC++;
    }
}

void code_handler_ret(){
    cpu.PC = readWordFromStack();
}

void code_handler_ret_cond(){
    if (*(uint8_t*)opcode_args_2[opcode]){
        cpu.PC = readWordFromStack();
    } else {
        cpu.PC++;
    }
}

void code_handler_ret_cond_inv(){
    if (!(*(uint8_t*)opcode_args_2[opcode])){
        cpu.PC = readWordFromStack();
    } else {
        cpu.PC++;
    }
}


void code_handler_add_w(){
    uint32_t result = *(uint16_t*)opcode_args_1[opcode] + *(uint16_t*)opcode_args_2[opcode];
    cpu.H = ((*(uint8_t*)opcode_args_1[opcode]&0xFF + *(uint8_t*)opcode_args_2[opcode]&0xFF) > 0xFF);
    *(uint16_t*)opcode_args_1[opcode] = result&0xFFFF;
    cpu.F_C = (result > 0xFFFF);
    cpu.F_N = 0;
}

void code_handler_rrca(){
    cpu.F_H = 0;
    cpu.F_N = 0;
    cpu.F_C = cpu.A&0x1;
    cpu.A >>= 1;
    cpu.A |= (cpu.F_C << 7);
}

void code_handler_rra(){
    cpu.F_H = 0;
    cpu.F_N = 0;
    int tmp = cpu.A&0x1;
    cpu.A >>= 1;
    cpu.A |= (cpu.F_C << 7);
    cpu.F_C = tmp;
}

void code_handler_di(){
    interrupts.enabled = 0;
}

void code_handler_jmp_rel_cond(){
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    uint8_t bit = (uint8_t) opcode_args_1[opcode];
    #pragma GCC diagnostic pop
    if ((bit && cpu.F_Z) || (!bit && cpu.F_C)){
        int8_t diff = 0;
        if (0b1000000&(*(uint8_t*)opcode_args_2[opcode])){
            
            diff = -~(*(uint8_t*)opcode_args_2[opcode])+1;
        } else {
            diff = (*(uint16_t*)opcode_args_2[opcode]);
        }
        cpu.PC += diff;
    } else {
        cpu.PC += 2;
    }

}

void code_handler_jmp_rel(){
    if (0b1000000&(*(uint16_t*)opcode_args_2[opcode])){
        int8_t diff = ~(*(uint8_t*)opcode_args_2[opcode])+1;
        cpu.PC -= diff;
    } else {
        cpu.PC += (*(uint16_t*)opcode_args_2[opcode]);
    }
}

void code_handler_jmp(){
    cpu.PC = *(uint16_t*)opcode_args_2[opcode];
}

void code_handler_jmp_rel_cond_inv(){
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    uint8_t bit = (uint8_t) opcode_args_1[opcode];
    #pragma GCC diagnostic pop
    if ((!bit && cpu.F_Z) || (bit && cpu.F_C)){
        cpu.PC += 2;
    } else {
        int8_t diff = 0;
        if (0b1000000&(*(uint8_t*)opcode_args_2[opcode])){
            
            diff = -~(*(uint8_t*)opcode_args_2[opcode])+1;
        } else {
            diff = (*(uint16_t*)opcode_args_2[opcode]);
        }
        cpu.PC += diff;
    }
}

void code_handler_push_w(){
    pushWordToStack(*(uint16_t*)opcode_args_2[opcode]);
}

void code_handler_push_w_af(){
    uint16_t val = (cpu.A << 8) + (0b10000000 * cpu.F_Z)+ (0b01000000 * cpu.F_N)+ (0b00100000 * cpu.F_H)+ (0b00010000 * cpu.F_C);
    pushWordToStack(val);
}

void code_handler_pop_w(){
    *(uint16_t*)opcode_args_2[opcode] = readWordFromStack();
}

void code_handler_pop_w_af(){
    uint16_t val = readWordFromStack();
    
    cpu.A = val>>8;
    cpu.F_Z = (val&0b10000000)?1:0;
    cpu.F_N = (val&0b01000000)?1:0;
    cpu.F_H = (val&0b00100000)?1:0;
    cpu.F_C = (val&0b00010000)?1:0;

}


// Increment byte
void code_handler_inc_b(){
    (*(uint8_t*)opcode_args_1[opcode])++;
    cpu.F_Z = !(*(uint8_t*)opcode_args_1[opcode]);
    cpu.F_H = cpu.F_Z;
    cpu.F_N = 0;
}

void code_handler_inc_hl(){
    uint8_t val = readByte(cpu.HL);
    val++;
    writeByte(cpu.HL, val);
    cpu.F_Z = !(val);
    cpu.F_H = cpu.F_Z;
    cpu.F_N = 0;
}

// Increment word
void code_handler_inc_w(){
    (*(uint16_t*)opcode_args_1[opcode])++;
}

// Decrement byte
void code_handler_dec_b(){
    (*(uint8_t*)opcode_args_1[opcode])--;
    cpu.F_Z = !(*(uint8_t*)opcode_args_1[opcode]);
    cpu.F_H = cpu.F_Z;
    cpu.F_N = 1;
}

void code_handler_dec_hl(){
    uint8_t val = readByte(cpu.HL);
    val--;
    writeByte(cpu.HL, val);
    cpu.F_Z = !(val);
    cpu.F_H = cpu.F_Z;
    cpu.F_N = 0;
}

// Decrement word
void code_handler_dec_w(){
    (*(uint16_t*)opcode_args_1[opcode])--;
}

// Add byte
void code_handler_add_b(){
    cpu.H = ((*(uint8_t*)opcode_args_1[opcode]&0xF + *(uint8_t*)opcode_args_2[opcode]&0xF) > 0xF);
    uint16_t result = *(uint8_t*)opcode_args_1[opcode] + *(uint8_t*)opcode_args_2[opcode];
    *(uint8_t*)opcode_args_1[opcode] = result&0xFF;
    cpu.F_Z = !(*(uint8_t*)opcode_args_1[opcode]);
    cpu.F_C = (result > 0xFF);
    cpu.F_N = 0;
}

void code_handler_add_b_indirect(){
    uint8_t arg2 = readByte(*(uint16_t*)opcode_args_2[opcode]);
    cpu.H = ((*(uint8_t*)opcode_args_1[opcode]&0xF + arg2&0xF) > 0xF);
    uint16_t result = *(uint8_t*)opcode_args_1[opcode] + arg2;
    *(uint8_t*)opcode_args_1[opcode] = result&0xFF;
    cpu.F_Z = !(*(uint8_t*)opcode_args_1[opcode]);
    cpu.F_C = (result > 0xFF);
    cpu.F_N = 0;
}

// Add byte with carry
void code_handler_adc_b(){
    cpu.H = ((*(uint8_t*)opcode_args_1[opcode]&0xF + *(uint8_t*)opcode_args_2[opcode]&0xF+ cpu.F_C) > 0xF);
    uint16_t result = *(uint8_t*)opcode_args_1[opcode] + *(uint8_t*)opcode_args_2[opcode]+cpu.F_C;
    *(uint8_t*)opcode_args_1[opcode] = result&0xFF;
    cpu.F_Z = !(*(uint8_t*)opcode_args_1[opcode]);
    cpu.F_N = 0;
    cpu.F_C = (result>0xFF);
}

void code_handler_adc_b_indirect(){
    uint8_t arg2 = readByte(*(uint16_t*)opcode_args_2[opcode]);
    cpu.H = ((*(uint8_t*)opcode_args_1[opcode]&0xF + arg2 + cpu.F_C) > 0xF);
    uint16_t result = *(uint8_t*)opcode_args_1[opcode] + arg2+cpu.F_C;
    *(uint8_t*)opcode_args_1[opcode] = result&0xFF;
    cpu.F_Z = !(*(uint8_t*)opcode_args_1[opcode]);
    cpu.F_N = 0;
    cpu.F_C = (result>0xFF);
}

void code_handler_cpl(){
    cpu.F_H = 1;
    cpu.F_N = 1;
    cpu.A = ~cpu.A;
}

// Sub byte
void code_handler_sub_b(){
    uint16_t result = *(uint8_t*)opcode_args_1[opcode] - *(uint8_t*)opcode_args_2[opcode];
    *(uint8_t*)opcode_args_1[opcode] = result&0xFF;
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 1;
    cpu.F_H = (*(uint8_t*)opcode_args_1[opcode]&0xF < *(uint8_t*)opcode_args_2[opcode]&0xF);
    cpu.F_C = (result > 0xFF);
}

void code_handler_sub_b_indirect(){
    uint8_t arg2 = readByte(*(uint16_t*)opcode_args_2[opcode]);
    uint16_t result = *(uint8_t*)opcode_args_1[opcode] - arg2;
    *(uint8_t*)opcode_args_1[opcode] = result&0xFF;
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 1;
    cpu.F_H = (*(uint8_t*)opcode_args_1[opcode]&0xF < arg2&0xF);
    cpu.F_C = (result > 0xFF);
}

// Sub byte with carry
void code_handler_sbc_b(){
    uint16_t result = *(uint8_t*)opcode_args_1[opcode] - *(uint8_t*)opcode_args_2[opcode] - cpu.F_C;
    *(uint8_t*)opcode_args_1[opcode] = result&0xFF;
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 1;
    cpu.F_H = (*(uint8_t*)opcode_args_1[opcode]&0xF < *(uint8_t*)opcode_args_2[opcode]&0xF);
    cpu.F_C = (result > 0xFF);
}

void code_handler_sbc_b_indirect(){
    uint8_t arg2 = readByte(*(uint16_t*)opcode_args_2[opcode]);
    uint16_t result = *(uint8_t*)opcode_args_1[opcode] - arg2 - cpu.F_C;
    *(uint8_t*)opcode_args_1[opcode] = result&0xFF;
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 1;
    cpu.F_H = (*(uint8_t*)opcode_args_1[opcode]&0xF < arg2&0xF);
    cpu.F_C = (result > 0xFF);
}


// bitwise AND
void code_handler_and_b(){
    cpu.A &= *(uint8_t*)opcode_args_2[opcode];
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 0;
    cpu.F_H = 1;
    cpu.F_C = 0;
}

void code_handler_and_b_indirect(){
    uint8_t arg2 = readByte(*(uint16_t*)opcode_args_2[opcode]);
    cpu.A &= arg2;
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 0;
    cpu.F_H = 1;
    cpu.F_C = 0;
}

// bitwise XOR
void code_handler_xor_b(){
    cpu.A ^= *(uint8_t*)opcode_args_2[opcode];
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 0;
    cpu.F_H = 0;
    cpu.F_C = 0;
}

void code_handler_xor_b_indirect(){
    uint8_t arg2 = readByte(*(uint16_t*)opcode_args_2[opcode]);
    cpu.A ^= arg2;
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 0;
    cpu.F_H = 0;
    cpu.F_C = 0;
}

// bitwise OR
void code_handler_or_b(){
    cpu.A |= *(uint8_t*)opcode_args_2[opcode];
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 0;
    cpu.F_H = 0;
    cpu.F_C = 0;
}

void code_handler_or_b_indirect(){
    uint8_t arg2 = readByte(*(uint16_t*)opcode_args_2[opcode]);
    cpu.A |= arg2;
    cpu.F_Z = !(cpu.A);
    cpu.F_N = 0;
    cpu.F_H = 0;
    cpu.F_C = 0;
}


// bitwise CP
void code_handler_cp_b(){
    cpu.F_Z = (*(uint8_t*)opcode_args_1[opcode] != *(uint8_t*)opcode_args_2[opcode]);
    cpu.F_N = 1;
    cpu.F_H = ((*(uint8_t*)opcode_args_1[opcode]&0xF) < (*(uint8_t*)opcode_args_2[opcode]&0xF));
    cpu.F_C = (*(uint8_t*)opcode_args_1[opcode] < *(uint8_t*)opcode_args_2[opcode]);
}

void code_handler_cp_b_indirect(){
    uint8_t arg2 = readByte(*(uint16_t*)opcode_args_2[opcode]);
    cpu.F_Z = (*(uint8_t*)opcode_args_1[opcode] != arg2);
    cpu.F_N = 1;
    cpu.F_H = ((*(uint8_t*)opcode_args_1[opcode]&0xF) < arg2);
    cpu.F_C = (*(uint8_t*)opcode_args_1[opcode] < arg2);
}

// RST
void code_handler_rst(){
    pushWordToStack(cpu.PC);
    // Yuck...
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    cpu.PC = (uint8_t)opcode_args_1[opcode] - 2;
    #pragma GCC diagnostic pop

}



void initCPU(){
    cpu.PC = 0x100;
    cpu.SP = 0xFFFE;
    
    REGISTER_OPCODE( 0,  4, 1, nop, "nop", 0, 0);
    REGISTER_OPCODE( 1, 12, 3, ld_w, "LD BC,d16", &cpu.BC, &cpu.d16);
    REGISTER_OPCODE(11, 12, 3, ld_w, "LD DE,d16", &cpu.DE, &cpu.d16);
    REGISTER_OPCODE(21, 12, 3, ld_w, "LD HL,d16", &cpu.HL, &cpu.d16);
    REGISTER_OPCODE(31, 12, 3, ld_w, "LD SP,d16", &cpu.SP, &cpu.d16);

    REGISTER_OPCODE( 3, 8, 1, inc_w, "INC BC", &cpu.BC, 0);
    REGISTER_OPCODE(13, 8, 1, inc_w, "INC DE", &cpu.DE, 0);
    REGISTER_OPCODE(23, 8, 1, inc_w, "INC HL", &cpu.HL, 0);
    REGISTER_OPCODE(33, 8, 1, inc_w, "INC SP", &cpu.SP, 0);
    REGISTER_OPCODE( 4, 4, 1, inc_b, "INC B", &cpu.B, 0);
    REGISTER_OPCODE(14, 4, 1, inc_b, "INC D", &cpu.D, 0);
    REGISTER_OPCODE(24, 4, 1,inc_b, "INC H", &cpu.H, 0);
    REGISTER_OPCODE(34, 12, 1,inc_hl, "INC (HL)", 0, 0);
    REGISTER_OPCODE( 5, 4, 1,dec_b, "DEC B", &cpu.B, 0);
    REGISTER_OPCODE(15, 4, 1,dec_b, "DEC D", &cpu.D, 0);
    REGISTER_OPCODE(25, 4, 1,dec_b, "DEC H", &cpu.H, 0);
    REGISTER_OPCODE(35, 12, 1,dec_hl, "DEC (HL)", 0, 0);

    REGISTER_OPCODE( 6, 8, 2, ld_b, "LD B,d8", &cpu.B, &cpu.d8);
    REGISTER_OPCODE(16, 8, 2, ld_b, "LD D,d8", &cpu.D, &cpu.d8);
    REGISTER_OPCODE(26, 8, 2, ld_b, "LD H,d8", &cpu.H, &cpu.d8);
    REGISTER_OPCODE(36, 12, 2, ld_b_indirect_write, "LD (HL),d8", &cpu.HL, &cpu.d8);

    REGISTER_OPCODE( E, 8, 2, ld_b, "LD C,d8", &cpu.C, &cpu.d8);
    REGISTER_OPCODE(1E, 8, 2, ld_b, "LD E,d8", &cpu.E, &cpu.d8);
    REGISTER_OPCODE(2E, 8, 2, ld_b, "LD L,d8", &cpu.L, &cpu.d8);
    REGISTER_OPCODE(3E, 8, 2, ld_b, "LD A,d8", &cpu.A, &cpu.d8);

    REGISTER_OPCODE( B, 8, 1,dec_w, "DEC BC", &cpu.BC, 0);
    REGISTER_OPCODE(1B, 8, 1,dec_w, "DEC DE", &cpu.DE, 0);
    REGISTER_OPCODE(2B, 8, 1,dec_w, "DEC HL", &cpu.HL, 0);
    REGISTER_OPCODE(3B, 8, 1,dec_w, "DEC SP", &cpu.SP, 0);
    REGISTER_OPCODE( C, 4, 1,inc_b, "INC C", &cpu.C, 0);
    REGISTER_OPCODE(1C, 4, 1,inc_b, "INC E", &cpu.E, 0);
    REGISTER_OPCODE(2C, 4, 1,inc_b, "INC L", &cpu.L, 0);
    REGISTER_OPCODE(3C, 4, 1,inc_b, "INC A", &cpu.A, 0);
    REGISTER_OPCODE( D, 4, 1,dec_b, "DEC C", &cpu.C, 0);
    REGISTER_OPCODE(1D, 4, 1,dec_b, "DEC E", &cpu.E, 0);
    REGISTER_OPCODE(2D, 4, 1,dec_b, "DEC L", &cpu.L, 0);
    REGISTER_OPCODE(3D, 4, 1,dec_b, "DEC A", &cpu.A, 0);



    REGISTER_OPCODE(40, 4, 1,ld_b, "LD B,B", &cpu.B, &cpu.B);
    REGISTER_OPCODE(41, 4, 1,ld_b, "LD B,C", &cpu.B, &cpu.C);
    REGISTER_OPCODE(42, 4, 1,ld_b, "LD B,D", &cpu.B, &cpu.D);
    REGISTER_OPCODE(43, 4, 1,ld_b, "LD B,E", &cpu.B, &cpu.E);
    REGISTER_OPCODE(44, 4, 1,ld_b, "LD B,H", &cpu.B, &cpu.H);
    REGISTER_OPCODE(45, 4, 1,ld_b, "LD B,L", &cpu.B, &cpu.L);
    REGISTER_OPCODE(46, 8, 1,ld_b_indirect_read, "LD B,(HL)", &cpu.B, &cpu.HL);
    REGISTER_OPCODE(47, 4, 1,ld_b, "LD B,A", &cpu.B, &cpu.A);

    REGISTER_OPCODE(48, 4, 1,ld_b, "LD C,B", &cpu.C, &cpu.B);
    REGISTER_OPCODE(49, 4, 1,ld_b, "LD C,C", &cpu.C, &cpu.C);
    REGISTER_OPCODE(4A, 4, 1,ld_b, "LD C,D", &cpu.C, &cpu.D);
    REGISTER_OPCODE(4B, 4, 1,ld_b, "LD C,E", &cpu.C, &cpu.E);
    REGISTER_OPCODE(4C, 4, 1,ld_b, "LD C,H", &cpu.C, &cpu.H);
    REGISTER_OPCODE(4D, 4, 1,ld_b, "LD C,L", &cpu.C, &cpu.L);
    REGISTER_OPCODE(4E, 8, 1,ld_b_indirect_read, "LD C,(HL)", &cpu.C, &cpu.HL);
    REGISTER_OPCODE(4F, 4, 1,ld_b, "LD C,A", &cpu.C, &cpu.A);

    REGISTER_OPCODE(50, 4, 1,ld_b, "LD D,B", &cpu.D, &cpu.B);
    REGISTER_OPCODE(51, 4, 1,ld_b, "LD D,C", &cpu.D, &cpu.C);
    REGISTER_OPCODE(52, 4, 1,ld_b, "LD D,D", &cpu.D, &cpu.D);
    REGISTER_OPCODE(53, 4, 1,ld_b, "LD D,E", &cpu.D, &cpu.E);
    REGISTER_OPCODE(54, 4, 1,ld_b, "LD D,H", &cpu.D, &cpu.H);
    REGISTER_OPCODE(55, 4, 1,ld_b, "LD D,L", &cpu.D, &cpu.L);
    REGISTER_OPCODE(56, 8, 1,ld_b_indirect_read, "LD D,(HL)", &cpu.D, &cpu.HL);
    REGISTER_OPCODE(57, 4, 1,ld_b, "LD D,A", &cpu.D, &cpu.A);

    REGISTER_OPCODE(58, 4, 1,ld_b, "LD E,B", &cpu.E, &cpu.B);
    REGISTER_OPCODE(59, 4, 1,ld_b, "LD E,C", &cpu.E, &cpu.C);
    REGISTER_OPCODE(5A, 4, 1,ld_b, "LD E,D", &cpu.E, &cpu.D);
    REGISTER_OPCODE(5B, 4, 1,ld_b, "LD E,E", &cpu.E, &cpu.E);
    REGISTER_OPCODE(5C, 4, 1,ld_b, "LD E,H", &cpu.E, &cpu.H);
    REGISTER_OPCODE(5D, 4, 1,ld_b, "LD E,L", &cpu.E, &cpu.L);
    REGISTER_OPCODE(5E, 8, 1,ld_b_indirect_read, "LD E,(HL)", &cpu.E, &cpu.HL);
    REGISTER_OPCODE(5F, 4, 1,ld_b, "LD E,A", &cpu.E, &cpu.A);

    REGISTER_OPCODE(60, 4, 1,ld_b, "LD H,B", &cpu.H, &cpu.B);
    REGISTER_OPCODE(61, 4, 1,ld_b, "LD H,C", &cpu.H, &cpu.C);
    REGISTER_OPCODE(62, 4, 1,ld_b, "LD H,D", &cpu.H, &cpu.D);
    REGISTER_OPCODE(63, 4, 1,ld_b, "LD H,E", &cpu.H, &cpu.E);
    REGISTER_OPCODE(64, 4, 1,ld_b, "LD H,H", &cpu.H, &cpu.H);
    REGISTER_OPCODE(65, 4, 1,ld_b, "LD H,L", &cpu.H, &cpu.L);
    REGISTER_OPCODE(66, 8, 1,ld_b_indirect_read, "LD H,(HL)", &cpu.H, &cpu.HL);
    REGISTER_OPCODE(67, 4, 1,ld_b, "LD H,A", &cpu.H, &cpu.A);

    REGISTER_OPCODE(68, 4, 1,ld_b, "LD L,B", &cpu.L, &cpu.B);
    REGISTER_OPCODE(69, 4, 1,ld_b, "LD L,C", &cpu.L, &cpu.C);
    REGISTER_OPCODE(6A, 4, 1,ld_b, "LD L,D", &cpu.L, &cpu.D);
    REGISTER_OPCODE(6B, 4, 1,ld_b, "LD L,E", &cpu.L, &cpu.E);
    REGISTER_OPCODE(6C, 4, 1,ld_b, "LD L,H", &cpu.L, &cpu.H);
    REGISTER_OPCODE(6D, 4, 1,ld_b, "LD L,L", &cpu.L, &cpu.L);
    REGISTER_OPCODE(6E, 8, 1,ld_b_indirect_read, "LD L,(HL)", &cpu.L, &cpu.HL);
    REGISTER_OPCODE(6F, 4, 1,ld_b, "LD L,A", &cpu.L, &cpu.A);


    REGISTER_OPCODE(70, 8, 1,ld_b_indirect_write, "LD (HL),B", &cpu.HL, &cpu.B);
    REGISTER_OPCODE(71, 8, 1,ld_b_indirect_write, "LD (HL),C", &cpu.HL, &cpu.C);
    REGISTER_OPCODE(72, 8, 1,ld_b_indirect_write, "LD (HL),D", &cpu.HL, &cpu.D);
    REGISTER_OPCODE(73, 8, 1,ld_b_indirect_write, "LD (HL),E", &cpu.HL, &cpu.E);
    REGISTER_OPCODE(74, 8, 1,ld_b_indirect_write, "LD (HL),H", &cpu.HL, &cpu.H);
    REGISTER_OPCODE(75, 8, 1,ld_b_indirect_write, "LD (HL),L", &cpu.HL, &cpu.L);
    // 0x76 is HALT
    REGISTER_OPCODE(77, 8, 1,ld_b_indirect_write, "LD (HL),A", &cpu.HL, &cpu.A);

    REGISTER_OPCODE(78, 4, 1,ld_b, "LD A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(79, 4, 1,ld_b, "LD A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(7A, 4, 1,ld_b, "LD A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(7B, 4, 1,ld_b, "LD A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(7C, 4, 1,ld_b, "LD A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(7D, 4, 1,ld_b, "LD A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(7E, 8, 1,ld_b_indirect_read, "LD A,(HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(7F, 4, 1,ld_b, "LD A,A", &cpu.A, &cpu.A);

    REGISTER_OPCODE(80, 4, 1,add_b, "ADD A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(81, 4, 1,add_b, "ADD A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(82, 4, 1,add_b, "ADD A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(83, 4, 1,add_b, "ADD A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(84, 4, 1,add_b, "ADD A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(85, 4, 1,add_b, "ADD A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(86, 8, 1,add_b_indirect, "ADD A,(HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(87, 4, 1,add_b, "ADD A,A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(C6, 8, 2,add_b, "ADD A,d8", &cpu.A, &cpu.d8);
    REGISTER_OPCODE(88, 4, 1,adc_b, "ADC A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(89, 4, 1,adc_b, "ADC A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(8A, 4, 1,adc_b, "ADC A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(8B, 4, 1,adc_b, "ADC A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(8C, 4, 1,adc_b, "ADC A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(8D, 4, 1,adc_b, "ADC A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(8E, 8, 1,adc_b_indirect, "ADC A,(HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(8F, 4, 1,adc_b, "ADC A,A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(CE, 8, 2,adc_b, "ADC A,d8", &cpu.A, &cpu.d8);

    REGISTER_OPCODE(90, 4, 1,sub_b, "SUB A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(91, 4, 1,sub_b, "SUB A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(92, 4, 1,sub_b, "SUB A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(93, 4, 1,sub_b, "SUB A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(94, 4, 1,sub_b, "SUB A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(95, 4, 1,sub_b, "SUB A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(96, 8, 1,sub_b_indirect, "SUB A,(HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(97, 4, 1,sub_b, "SUB A,A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(D6, 8, 2,sub_b, "SUB A,d8", &cpu.A, &cpu.d8);
    REGISTER_OPCODE(98, 4, 1,sbc_b, "SBC A,B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(99, 4, 1,sbc_b, "SBC A,C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(9A, 4, 1,sbc_b, "SBC A,D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(9B, 4, 1,sbc_b, "SBC A,E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(9C, 4, 1,sbc_b, "SBC A,H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(9D, 4, 1,sbc_b, "SBC A,L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(9E, 8, 1,sbc_b_indirect, "SBC A,(HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(9F, 4, 1,sbc_b, "SBC A,A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(DE, 8, 2,sbc_b, "SBC A,d8", &cpu.A, &cpu.d8);

    REGISTER_OPCODE(A0, 4, 1,and_b, "AND B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(A1, 4, 1,and_b, "AND C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(A2, 4, 1,and_b, "AND D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(A3, 4, 1,and_b, "AND E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(A4, 4, 1,and_b, "AND H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(A5, 4, 1,and_b, "AND L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(A6, 8, 1,and_b_indirect, "AND (HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(A7, 4, 1,and_b, "AND A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(E6, 8, 2,and_b, "AND d8", &cpu.A, &cpu.d8);
    REGISTER_OPCODE(A8, 4, 1,xor_b, "XOR B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(A9, 4, 1,xor_b, "XOR C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(AA, 4, 1,xor_b, "XOR D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(AB, 4, 1,xor_b, "XOR E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(AC, 4, 1,xor_b, "XOR H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(AD, 4, 1,xor_b, "XOR L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(AE, 8, 1,xor_b_indirect, "XOR (HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(AF, 4, 1,xor_b, "XOR A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(EE, 8, 2,xor_b, "XOR d8", &cpu.A, &cpu.d8);

    REGISTER_OPCODE(B0, 4, 1,or_b, "OR B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(B1, 4, 1,or_b, "OR C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(B2, 4, 1,or_b, "OR D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(B3, 4, 1,or_b, "OR E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(B4, 4, 1,or_b, "OR H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(B5, 4, 1,or_b, "OR L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(B6, 8, 1,or_b_indirect, "OR (HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(B7, 4, 1,or_b, "OR A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(F6, 8, 2,or_b, "OR d8", &cpu.A, &cpu.d8);
    REGISTER_OPCODE(B8, 4, 1,cp_b, "CP B", &cpu.A, &cpu.B);
    REGISTER_OPCODE(B9, 4, 1,cp_b, "CP C", &cpu.A, &cpu.C);
    REGISTER_OPCODE(BA, 4, 1,cp_b, "CP D", &cpu.A, &cpu.D);
    REGISTER_OPCODE(BB, 4, 1,cp_b, "CP E", &cpu.A, &cpu.E);
    REGISTER_OPCODE(BC, 4, 1,cp_b, "CP H", &cpu.A, &cpu.H);
    REGISTER_OPCODE(BD, 4, 1,cp_b, "CP L", &cpu.A, &cpu.L);
    REGISTER_OPCODE(BE, 8, 1,cp_b_indirect, "CP (HL)", &cpu.A, &cpu.HL);
    REGISTER_OPCODE(BF, 4, 1,cp_b, "CP A", &cpu.A, &cpu.A);
    REGISTER_OPCODE(FE, 8, 2,cp_b, "CP d8", &cpu.A, &cpu.d8);

    REGISTER_OPCODE(C7, 16, 0, rst, "RST 00H", 0x0, 0);
    REGISTER_OPCODE(D7, 16, 0, rst, "RST 10H", 0x10, 0);
    REGISTER_OPCODE(E7, 16, 0, rst, "RST 20H", 0x20, 0);
    REGISTER_OPCODE(F7, 16, 0, rst, "RST 30H", 0x30, 0);
    REGISTER_OPCODE(CF, 16, 0, rst, "RST 08H", 0x8, 0);
    REGISTER_OPCODE(DF, 16, 0, rst, "RST 18H", 0x18, 0);
    REGISTER_OPCODE(EF, 16, 0, rst, "RST 28H", 0x28, 0);
    REGISTER_OPCODE(FF, 16, 0, rst, "RST 38H", 0x38, 0);

    REGISTER_OPCODE(8, 20, 3, ld_w_indirect, "LD (a16),SP", &cpu.d16, &cpu.SP);
    REGISTER_OPCODE( 2, 8, 1,ld_w_indirect, "LD (BC),A", &cpu.BC, &cpu.A);
    REGISTER_OPCODE(12, 8, 1,ld_w_indirect, "LD (DE),A", &cpu.DE, &cpu.A);
    REGISTER_OPCODE(22, 8, 1,ldi_w_indirect, "LD (HL+),A", &cpu.HL, &cpu.A);
    REGISTER_OPCODE(32, 8, 1,ldd_w_indirect, "LD (HL-),A", &cpu.HL, &cpu.A);
    REGISTER_OPCODE(EA, 16, 3, ld_w_indirect, "LD (a16),A", &cpu.d16, &cpu.A);
    
    REGISTER_OPCODE(20, 12, 0, jmp_rel_cond_inv, "JR NZ,r8", 0, &cpu.d8);
    REGISTER_OPCODE(30, 12, 0, jmp_rel_cond_inv, "JR NC,r8", 1, &cpu.d8);
    REGISTER_OPCODE(18, 12, 0, jmp_rel_cond, "JR Z,r8", 0, &cpu.d8);
    REGISTER_OPCODE(28, 12, 0, jmp_rel_cond, "JR C,r8", 1, &cpu.d8);
    REGISTER_OPCODE(38, 12, 0, jmp_rel, "JR r8", 0, &cpu.d8);
    REGISTER_OPCODE(F, 4, 1,rrca, "RRCA", 0,0);
    REGISTER_OPCODE(1F, 4, 1,rra, "RRA", 0,0);
    REGISTER_OPCODE(F3, 4, 1,di, "DI", 0,0);
    REGISTER_OPCODE(C3, 12, 0, jmp, "JP a16", 0, &cpu.d16);

    REGISTER_OPCODE( 9, 8, 1,add_w, "ADD HL,BC", &cpu.HL, &cpu.BC);
    REGISTER_OPCODE(19, 8, 1,add_w, "ADD HL,DE", &cpu.HL, &cpu.DE);
    REGISTER_OPCODE(29, 8, 1,add_w, "ADD HL,HL", &cpu.HL, &cpu.HL);
    REGISTER_OPCODE(39, 8, 1,add_w, "ADD HL,SP", &cpu.HL, &cpu.SP);
    REGISTER_OPCODE(2F, 4, 1, cpl, "CPL", 0,0);

    REGISTER_OPCODE(E0, 12, 2, ldh_b_indirect_w, "LDH (a8),A", &cpu.d8, &cpu.A);
    REGISTER_OPCODE(F0, 12, 2, ldh_b_indirect_r, "LDH A,(a8)", &cpu.d8, &cpu.A);
    REGISTER_OPCODE(2A, 8, 1, lda_hp_indirect, "LD A,(HL+)", &cpu.HL, &cpu.A);
    REGISTER_OPCODE(3A, 8, 1, lda_hn_indirect, "LD A,(HL-)", &cpu.HL, &cpu.A);
    REGISTER_OPCODE(CD, 24, 0, call, "CALL a16", &cpu.d16, 0);
    REGISTER_OPCODE(CC, 24, 0, call_cond, "CALL Z,a16", &cpu.d16, &cpu.F_Z);
    REGISTER_OPCODE(DC, 24, 0, call_cond, "CALL C,a16", &cpu.d16, &cpu.F_C);
    REGISTER_OPCODE(C4, 24, 0, call_cond_inv, "CALL NZ,a16", &cpu.d16, &cpu.F_Z);
    REGISTER_OPCODE(D4, 24, 0, call_cond_inv, "CALL NC,a16", &cpu.d16, &cpu.F_C);
    REGISTER_OPCODE(C9, 16, 0, ret, "RET", 0, 0);
    REGISTER_OPCODE(C8, 20, 0, ret_cond, "RET Z", 0, &cpu.F_Z);
    REGISTER_OPCODE(D8, 20, 0, ret_cond, "RET C", 0, &cpu.F_C);
    REGISTER_OPCODE(C0, 20, 0, ret_cond_inv, "RET NZ", 0, &cpu.F_Z);
    REGISTER_OPCODE(D0, 20, 0, ret_cond_inv, "RET NC", 0, &cpu.F_C);
    REGISTER_OPCODE(C5, 16, 1, push_w, "PUSH BC", 0, &cpu.BC);
    REGISTER_OPCODE(D5, 16, 1, push_w, "PUSH DE", 0, &cpu.DE);
    REGISTER_OPCODE(E5, 16, 1, push_w, "PUSH HL", 0, &cpu.HL);
    REGISTER_OPCODE(F5, 16, 1, push_w_af, "PUSH AF", 0, 0);
    REGISTER_OPCODE(C1, 12, 1, pop_w, "POP BC", 0, &cpu.BC);
    REGISTER_OPCODE(D1, 12, 1, pop_w, "POP DE", 0, &cpu.DE);
    REGISTER_OPCODE(E1, 12, 1, pop_w, "POP HL", 0, &cpu.HL);
    REGISTER_OPCODE(F1, 12, 1, pop_w_af, "POP AF", 0, 0);

    REGISTER_OPCODE(0A, 8, 1,ld_w_indirect_read, "LD A,(BC)", &cpu.A, &cpu.BC);
    REGISTER_OPCODE(1A, 8, 1,ld_w_indirect_read, "LD A,(DE)", &cpu.A, &cpu.DE);
    REGISTER_OPCODE(F2, 8, 1,ld_b_indirect_read, "LD A,(C)", &cpu.A, &cpu.C);
    REGISTER_OPCODE(E2, 8, 1,ld_b_indirect_write, "LD (C),A", &cpu.A, &cpu.C);

}

void executeOpcode(){
    opcode = readByte(cpu.PC);
    if (opcode_implemented[opcode] == 0){
        consoleError("Opcode 0x%X at 0x%X is not implemented\n", opcode, cpu.PC);
        return;
    }
    // Prefetch values
    cpu.d8 = readByte(cpu.PC+1);
    cpu.d16 = (readByte(cpu.PC+2) << 8) + readByte(cpu.PC+1);
    // TODO fix every reference to HL_VAL
    opcode_handlers[opcode]();
    printf("0x%02X : %s\n", opcode, opcode_names[opcode]);
    
    // What a shitty hack
    char buffer[512];
    char secondBuffer[512];
    strcpy(buffer, opcode_names[opcode]);
    void *toReplace = NULL;
    // Replace 8 bit
    for(int i = 0; i < strlen(buffer)-1; i++){
        if (buffer[i+1] == '8' && (buffer[i] == 'r' || buffer[i] == 'd' || buffer[i] == 'a')){
            strncpy(secondBuffer, buffer, i);
            secondBuffer[i] = '\0';
            strcat(secondBuffer+i, "0x%02X");
            if (buffer[i+2] != '\0'){
                strcat(secondBuffer+i+4, buffer+i+2);
            } 
            toReplace = &cpu.d8;
        }
    }

    // Replace 16 bit
    for(int i = 0; i < strlen(buffer)-2; i++){
        if (buffer[i+1] == '1' && buffer[i+2] == '6' && (buffer[i] == 'r' || buffer[i] == 'd' || buffer[i] == 'a')){
            strncpy(secondBuffer, buffer, i);
            secondBuffer[i] = '\0';
            strcat(secondBuffer+i, "0x%04X");
            if (buffer[i+2] != '\0'){
                strcat(secondBuffer+i+4, buffer+i+3);
            } 
            toReplace = &cpu.d16;
        }
    }

    // Naughty overflow
    char formattedString[512];
    if (toReplace != NULL){
        sprintf(formattedString, secondBuffer, *(uint16_t*)toReplace);
    } else {
        sprintf(formattedString, "%s", opcode_names[opcode]);
    }

/*
    sprintf(formattedString, "%s", opcode_names[opcode]);
    if (strstr(opcode_names[opcode], "r8") || strstr(opcode_names[opcode], "d8") || strstr(opcode_names[opcode], "a8")){
        sprintf(formattedString, "%s 0x%X", opcode_names[opcode], cpu.d8);
    }
    if (strstr(opcode_names[opcode], "r16") || strstr(opcode_names[opcode], "d16") || strstr(opcode_names[opcode], "a16")){
        sprintf(formattedString, "%s 0x%X", opcode_names[opcode], cpu.d16);
    }*/
    consoleLog("%s", formattedString);

    cpu.PC += opcode_length[opcode];
}


