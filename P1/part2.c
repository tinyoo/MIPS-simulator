#include <stdio.h> /* for stderr */
#include <stdlib.h> /* for exit() */
#include "types.h"
#include "utils.h"
#include "mips.h"

void execute_rtype(Instruction, Processor *);
void execute_itype(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_j(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);

void execute_instruction(Instruction inst, Processor *p UNUSED, Byte *memory UNUSED) {    
    
    switch(inst.opcode.opcode) { 
        case(0x00):
            execute_rtype(inst, p);
            break;

        case(0x1c):
            execute_rtype(inst, p);
            break;

        case(0x23):
            execute_load(inst, p, memory);
            break;

        case(0x30):
        case(0x31):
        case(0x21):
        case(0x32):
        case(0x33):
        case(0x35):
            execute_itype(inst, p);
            break;

        case(0x2b):
            execute_store(inst, p, memory);
            break;

        case(0x04):
        case(0x01):
        case(0x07):
            execute_branch(inst, p);
            break;

        case(0x02):
            execute_j(inst, p);
            break;

        default: /* undefined opcode */
            handle_invalid_instruction(inst);
            exit(-1);
            break;
    }
}


void execute_rtype(Instruction inst, Processor *p UNUSED) {

    switch(inst.rtype.funct) {
        case(0x20):   /* add */
            p -> R[inst.rtype.rd] = (sWord)p -> R[inst.rtype.rs] + (sWord)p -> R[inst.rtype.rt];
            break;
        case(0x02):     
            if ( inst.rtype.opcode == 0x1c )    /* mul */
                p -> R[inst.rtype.rd] = (sWord)p -> R[inst.rtype.rs] * (sWord)p -> R[inst.rtype.rt];
            else if ( inst.rtype.opcode == 0x00 )    /* srl */
                p -> R[inst.rtype.rd] = p -> R[inst.rtype.rt] >> (Word)inst.rtype.shamt;
            break;
        case(0x22):     /* sub */
            p -> R[inst.rtype.rd] = (sWord)p -> R[inst.rtype.rs] - (sWord)p -> R[inst.rtype.rt];
            break;
        case(0x00):     /* sll */
        {
            if ( inst.rtype.rs == 0x00 )   /* sll */
                p -> R[inst.rtype.rd] = p -> R[inst.rtype.rt] << (unsigned)bitSigner(inst.rtype.shamt, 5);
            if ( inst.rtype.rs == 0x00 )   /* nop */ {}
            break;
        }
        case(0x2a):     /* slt */
            p -> R[inst.rtype.rd] = (sWord)(signed)bitSigner(p -> R[inst.rtype.rs], 5) < (sWord)(signed)bitSigner(p -> R[inst.rtype.rt], 5) ? 1 : 0;
            break;
        case(0x03):     /* sra */
            p -> R[inst.rtype.rd] = (sWord)bitSigner(p -> R[inst.rtype.rt], 16) >> (Word)inst.rtype.shamt;
            break;
        case(0x24):     /* and */
            p -> R[inst.rtype.rd] = p -> R[inst.rtype.rs] & p -> R[inst.rtype.rt];
            break;
        case(0x27):     /* nor */
            p -> R[inst.rtype.rd] = ~(p -> R[inst.rtype.rs] | p -> R[inst.rtype.rt]);
            break;
        case(0x08):     /* jr  */
        {
            Address nextPC UNUSED;
            nextPC = 0;
            nextPC = (sWord)(p -> R[inst.rtype.rs]);
            p -> R[inst.rtype.rd] = (sWord)(bitSigner(p -> PC, 32) + 4);
            p -> PC = nextPC;
            break;
        }
        case(0x0d):     /* break */
            break;
        default:        /* undefined opcode */
            handle_invalid_instruction(inst);
            exit(-1);
            break;
    }
    p -> PC += 4;
}


void execute_itype(Instruction inst, Processor *p UNUSED) {
    switch(inst.itype.opcode) {
        /* addi */
        case(0x30):
            p -> R[inst.itype.rt] = (sWord)(p -> R[inst.itype.rs] + (signed)bitSigner(inst.itype.offset, 16));
            break;

        /* subi */
        case(0x31):
            p -> R[inst.itype.rt] = (sWord)(p -> R[inst.itype.rs] - (signed)bitSigner(inst.itype.offset, 16));
            break;

        /* muli */
        case(0x21):
            p -> R[inst.itype.rt] = (sWord)(p -> R[inst.itype.rs] * (signed)bitSigner(inst.itype.offset, 16));
            break;

        /* andi */
        case(0x32):
            p -> R[inst.itype.rt] = p -> R[inst.itype.rs] & bitSigner(inst.itype.offset, 16);
            break;

        /* nori */
        case(0x33):
            p -> R[inst.itype.rt] = ! (p -> R[inst.itype.rs] | bitSigner(inst.itype.offset, 16));
            break;

        /* slti */
        case(0x35):
            p -> R[inst.itype.rt] = (sWord)bitSigner(p -> R[inst.itype.rs],16) < (signed)(bitSigner(inst.itype.offset, 16)) ? 1 : 0;
            break;

        default:
            handle_invalid_instruction(inst);
            exit(-1);
            break;
    }
    p -> PC += 4;
}


void execute_branch(Instruction inst, Processor *p UNUSED) {
    int branchaddr UNUSED;
    branchaddr = 0;
    
    switch(inst.itype.opcode) {
        /* beq */
        case(0x04):
            if ( (signed)(p -> R[inst.itype.rs]) == (signed)(p -> R[inst.itype.rt]) )
                branchaddr += ((sHalf)get_branch_offset(inst)+4);
            else
                branchaddr += 4;
            break;

        /* bltz */
        case(0x01):
            if ( (signed)(p -> R[inst.itype.rs]) < 0 )
                branchaddr += ((sHalf)get_branch_offset(inst)+4);
            else
                branchaddr += 4;
            break;

        /* bgtz */
        case(0x07):
            if ( (signed)(p -> R[inst.itype.rs]) > 0 )
                branchaddr += ((sHalf)get_branch_offset(inst)+4);
            else
                branchaddr += 4;
            break;

        default:
            handle_invalid_instruction(inst);
            exit(-1);
            break;
    }
    p -> PC += branchaddr;
}

void execute_load(Instruction inst, Processor *p UNUSED, Byte *memory UNUSED) {
    Address address;
    Word word;
    switch(inst.itype.opcode) {
        /* lb */
        case(0x23):
            address = (sWord)p -> R[inst.itype.rs] + bitSigner(inst.itype.offset, 16);
            word = load(memory, address, LENGTH_BYTE, 0);
            p -> R[inst.itype.rt] = (sByte)bitSigner(word, 8);
            /*R[rt] ← SignExt(Mem(R[rs](base) + offset, byte))*/
            break;

        default:
            handle_invalid_instruction(inst);
            exit(-1);
            break;
    }
    p -> PC += 4;
}

void execute_store(Instruction inst, Processor *p UNUSED, Byte *memory UNUSED) {
    Address address;
    switch(inst.itype.opcode) {
        /* sb */
        case(0x2b):
            address = (sWord)p -> R[inst.itype.rs] + (sWord)get_store_offset(inst);
            store(memory, address, LENGTH_BYTE, (sWord)p -> R[inst.itype.rt], 0);
            break;

        default:
            handle_invalid_instruction(inst);
            exit(-1);
            break;
    }
    p -> PC += 4;
}

void execute_j(Instruction inst UNUSED, Processor *p UNUSED) {
    Address nextPC UNUSED;
    nextPC = 0;
    nextPC = ((((p->PC+4)>>28) & 0xf) << 28) | (signed)(bitSigner(get_jump_offset(inst), 32));
    p -> PC = nextPC; 
}

/* Checks that the address is aligned correctly */
int check(Address address, Alignment alignment) {
    if(address > 0 && address < MEMORY_SPACE) {
        if(alignment == LENGTH_BYTE) {
            return 1;
        }
        else if(alignment == LENGTH_HALF_WORD) {
            return address % 2 == 0;
        }
        else if(alignment == LENGTH_WORD) {
            return address % 4 == 0;
        }
    }
    return 0;
}

void store(Byte *memory UNUSED, Address address, Alignment alignment, Word value UNUSED, int check_align) {
    if((check_align && !check(address, alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_write(address);
    }

    else {
        switch(alignment) {
            case(LENGTH_BYTE):
                memory[address] = (value & 0xff);
                break;

            case(LENGTH_HALF_WORD):
                memory[address] = (value & 0xff);
                memory[address + 1] = ((value >> 8) & 0xff);
                break;

            case(LENGTH_WORD):
                memory[address] = (value & 0xff);
                memory[address + 1] = ((value >> 8) & 0xff);
                memory[address + 2] = ((value >> 16) & 0xff);
                memory[address + 3] = ((value >> 24) & 0xff);   /* add Byte no change */
                break;

            default: 
                break;
        }
    }
}

Word load(Byte *memory UNUSED, Address address, Alignment alignment, int check_align) {
    Word data UNUSED = 0; /* initialize our return value to zero */

    if((check_align && !check(address, alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_read(address);
    }
    else{
        switch (alignment){
            case (LENGTH_BYTE):
                data |= memory[address];
                break;
            
            case(LENGTH_HALF_WORD):
                data |= memory[address];
                data |= (memory[address + 1] << 8);
                /* data = bitSigner(data, 16);*/
                break;

            case(LENGTH_WORD):
                data |= memory[address];
                data |= (memory[address + 1] << 8);
                data |= (memory[address + 2] << 16);
                data |= (memory[address + 3] << 24);
                break;

            default:
                break;
        }
    }
    return data;
}

