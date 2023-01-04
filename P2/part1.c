#include <stdio.h> /* for stderr */
#include <stdlib.h> /* for exit() */
#include "types.h"
#include "utils.h"

void print_rtype(char *, Instruction, int index);
void print_break(char *, Instruction);
void print_nop(char *, Instruction, int index);
void print_itype(char *, Instruction, int index);
void print_load(char *, Instruction, int index);
void print_store(char *, Instruction, int index);
void print_branch(char *, Instruction, int index);
void write_mul(Instruction, int index);
void write_rtype(Instruction, int index);
void write_itype(Instruction, int index); 
void write_load(Instruction, int index);
void write_store(Instruction, int index);
void write_beq(Instruction, int index);
void write_bltz(Instruction, int index);
void write_bgtz(Instruction, int index);
void write_j(Instruction, int index);


void decode_instruction(Instruction inst, int index) {
    switch(inst.opcode.opcode) {
        
        case(0x1c): {   write_mul(inst, index);                    break; }

        case(0x00): {   write_rtype(inst, index);                  break; }
        
        case(0x23): {   write_load(inst, index);                   break; }

        case(0x2b): {   write_store(inst, index);                  break; }

        case(0x02): {   write_j   (inst, index);                   break; }

        case(0x04): {   write_beq(inst, index);                    break; }

        case(0x01): {   write_bltz(inst, index);                   break; }

        case(0x07): {   write_bgtz(inst, index);                   break; }

        case(0x30): {   write_itype(inst, index);                  break; }

        case(0x31): {   write_itype(inst, index);                  break; }

        case(0x32): {   write_itype(inst, index);                  break; }

        case(0x33): {   write_itype(inst, index);                  break; }

        case(0x35): {   write_itype(inst, index);                  break; }

        case(0x21): {   write_itype(inst, index);                  break; }

        /* undefined opcode */
        default:    {   handle_invalid_instruction(inst);          break; }  
    }
}

void write_mul(Instruction inst, int index) {
    if ( (inst.rtype.shamt == 0x00) & (inst.rtype.funct == 0x02) ) {
        print_rtype("MUL", inst, index);
    }
}

void write_rtype(Instruction inst, int index) {
    if ( inst.rtype.funct == 0x0d )                print_break("BREAK", inst);
    else if ( (inst.rtype.funct == 0x00)
            && (inst.rtype.shamt == 0x00) 
            && (inst.rtype.rd == 0x00)
            && (inst.rtype.rt == 0x00)
            && (inst.rtype.rs == 0x00) )
                                                   print_nop("NOP", inst, index);
    else if ( inst.rtype.funct == 0x08 )           print_rtype("JR", inst, index);
    else {
        if ( inst.rtype.shamt == 0 ) {
            /* add */
            if ( inst.rtype.funct == 0x20 )        print_rtype("ADD", inst, index);
            /* and */
            else if ( inst.rtype.funct == 0x24 )   print_rtype("AND", inst, index);
            /* sub */
            else if ( inst.rtype.funct == 0x22 )   print_rtype("SUB", inst, index);
            /* slt */
            else if ( inst.rtype.funct == 0x2a )   print_rtype("SLT", inst, index);
            /* nor */
            else if ( inst.rtype.funct == 0x27 )   print_rtype("NOR", inst, index);
            /* undefined funct */
            else                                   handle_invalid_instruction(inst);
        }
        else if ( inst.rtype.rs == 0x00 ) {
            /* sll */
            if ( inst.rtype.funct == 0x00 )        print_rtype("SLL", inst, index);
            /* sra */
            else if ( inst.rtype.funct == 0x03 )   print_rtype("SRA", inst, index);
            /* srl */
            else if ( inst.rtype.funct == 0x02 )   print_rtype("SRL", inst, index);
            /* undefined funct */
            else                                   handle_invalid_instruction(inst);
        }
        else                                       handle_invalid_instruction(inst);
    }
}

void write_load(Instruction inst, int index) {
    print_load("LW", inst, index);
}

void write_store(Instruction inst, int index) {
    print_store("SW", inst, index);
}

void write_j(Instruction inst UNUSED, int index) {
    if (index == 1)
        printf(J_FORMAT, get_jump_offset(inst));
    else if (index == 2)
        printf(J_FORMAT2, get_jump_offset(inst));
}

void write_beq(Instruction inst UNUSED, int index) {
    print_branch("BEQ", inst, index);
}

void write_bltz(Instruction inst UNUSED, int index) {
    print_branch("BLTZ", inst, index);
}

void write_bgtz(Instruction inst UNUSED, int index) {
    print_branch("BGTZ", inst, index);
}

void write_itype(Instruction inst, int index) {
    switch(inst.itype.opcode) {
        /* addi */
        case(0x30):
        {
            print_itype("ADD", inst, index);
            break;
        }

        /* subi */
        case(0x31):
        {
            print_itype("SUB", inst, index);
            break;
        }

        /* muli */
        case(0x21):
        {
            print_itype("MUL", inst, index);
            break;
        }

        /* andi */
        case(0x32):
        {
            print_itype("AND", inst, index);
            break;
        }

        /* nori */
        case(0x33):
        {
            print_itype("NOR", inst, 1);
            break;
        }

        /* slti */
        case(0x35):
        {
            print_itype("SLT", inst, index);
            break;
        }

        default:
            handle_invalid_instruction(inst);
            break;  
    }
}


void print_rtype(char *name UNUSED, Instruction inst UNUSED, int index) {
    if (index == 1) {
        if ( (inst.rtype.rt == 0x00) && (inst.rtype.rd == 0x00) && (inst.rtype.funct == 0x08) )
            printf(JR_FORMAT, (unsigned)inst.rtype.rs);
        else if ( (inst.rtype.shamt == 0x00) && (inst.rtype.funct != 0x08) )
            printf(RTYPE_FORMAT, name, (unsigned)inst.rtype.rd, (unsigned)inst.rtype.rs, (unsigned)inst.rtype.rt);
        else
            printf(ITYPE_FORMAT, name, inst.rtype.rd, inst.rtype.rt, inst.rtype.shamt);
    }
    else if (index == 2) {
        if ( (inst.rtype.rt == 0x00) && (inst.rtype.rd == 0x00) && (inst.rtype.funct == 0x08) )
            printf(JR_FORMAT2, (unsigned)inst.rtype.rs);
        else if ( (inst.rtype.shamt == 0x00) && (inst.rtype.funct != 0x08) )
            printf(RTYPE_FORMAT2, name, (unsigned)inst.rtype.rd, (unsigned)inst.rtype.rs, (unsigned)inst.rtype.rt);
        else
            printf(ITYPE_FORMAT2, name, inst.rtype.rd, inst.rtype.rt, inst.rtype.shamt);
    }
}

void print_break(char *name UNUSED, Instruction inst UNUSED) {
    printf(BREAK_FORMAT);
}

void print_nop(char *name UNUSED, Instruction inst UNUSED, int index) {
    if (index == 1)
        printf(NOP_FORMAT);
    else if (index == 2)
        printf(NOP_FORMAT2);
}

void print_branch(char *name UNUSED, Instruction inst UNUSED, int index) {
    switch(inst.itype.opcode) {
        case(0x04):
        {
            if (index == 1)
                printf(ITYPE_FORMAT, name, inst.itype.rs, inst.itype.rt, get_branch_offset(inst));
            else if (index == 2)
                printf(ITYPE_FORMAT2, name, inst.itype.rs, inst.itype.rt, get_branch_offset(inst));
            break;
        }
        case(0x01):
        case(0x07):
        {
            if (index == 1)
                printf(UTYPE_FORMAT, name, (unsigned)inst.itype.rs, get_branch_offset(inst));
            else if (index == 2)
                printf(UTYPE_FORMAT2, name, (unsigned)inst.itype.rs, get_branch_offset(inst));
            break;
        }
        default:    
        {   
            handle_invalid_instruction(inst);   
            break; 
        } 
    }
    
}

void print_itype(char *name UNUSED, Instruction inst UNUSED, int index) {
    if (index == 1)
        printf(ITYPE_FORMAT, name, inst.itype.rt, inst.itype.rs, bitSigner(inst.itype.offset,16));    /********/
    else if (index == 2)
        printf(ITYPE_FORMAT2, name, inst.itype.rt, inst.itype.rs, bitSigner(inst.itype.offset,16));
}

void print_load(char *name UNUSED, Instruction inst UNUSED, int index) {
    if (index == 1)
        printf(MEM_FORMAT, name, inst.itype.rt, bitSigner(inst.itype.offset,16), inst.itype.rs);  /******/
    else if (index == 2)
        printf(MEM_FORMAT2, name, inst.itype.rt, bitSigner(inst.itype.offset,16), inst.itype.rs);
}

void print_store(char *name UNUSED, Instruction inst UNUSED, int index) {
    if (index == 1)
        printf(MEM_FORMAT, name, inst.itype.rt, bitSigner(inst.itype.offset,16), inst.itype.rs);
    else if (index == 2)
        printf(MEM_FORMAT2, name, inst.itype.rt, bitSigner(inst.itype.offset,16), inst.itype.rs);
}

