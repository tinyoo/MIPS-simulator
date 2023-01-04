#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int isBranch(Instruction *inst) {
    return ( ( inst->opcode.opcode == 0x04 ) /* BEQ */
            || ( (inst->opcode.opcode == 0x01)  && (inst->itype.rt == 0x00) ) /* BLTZ*/
            || ( (inst->opcode.opcode == 0x07)  && (inst->itype.rt == 0x00) ) /* BGTZ */ );
}

int isJ(Instruction *inst) {
    return ( (inst->opcode.opcode == 0x02) 
            || ((inst->opcode.opcode == 0x00) && (inst->rtype.funct == 0x08)) );  /* J */ /* JR */
}

/* MULI, ADDI, SUBI, ANDI, NORI, SLTI */
int isOPI(Instruction *inst) {
    return ( (inst->opcode.opcode == 0x21) || (inst->opcode.opcode == 0x30) 
            || (inst->opcode.opcode == 0x31) || (inst->opcode.opcode == 0x32) 
            || (inst->opcode.opcode == 0x33) || (inst->opcode.opcode == 0x35) );
}

int isR(Instruction *inst) {
    return ( ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x20) ) /* ADD */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x22) ) /* SUB */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x24) ) /* AND */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x27) ) /* NOR */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x2a) ) /* SLT */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x00) ) /* SLL */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x02) ) /* SRL */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x03) ) /* SRA */
            || ( (inst->opcode.opcode == 0x1c) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x02) ) /* MUL */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.rt == 0x00) 
                                               && (inst->rtype.rd == 0x00) && (inst->rtype.funct == 0x00) )  /* NOP */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.funct == 0x0d) ) /* BREAK */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.funct == 0x08) ) /* JR */
    );
}

int isShift(Instruction *inst) {
    return ( ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x00) ) /* SLL */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x02) ) /* SRL */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x03) ) /* SRA */ );
}

int isI(Instruction *inst) {
    return ( isOPI(inst) || isInMEM(inst) 
            || (inst->opcode.opcode == 0x04) /* BEQ */
            || (inst->opcode.opcode == 0x01) /* BLTZ*/
            || (inst->opcode.opcode == 0x07) /* BGTZ */);
}

/* ADD, SUB, AND, NOR, SLT, ADDI, SUBI, ANDI, NORI, SLTI */
int isInALU(Instruction *inst) {
    return ( ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x20) ) /* ADD */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x22) ) /* SUB */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x24) ) /* AND */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x27) ) /* NOR */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x2a) ) /* SLT */
            || (inst->opcode.opcode == 0x30) || (inst->opcode.opcode == 0x31) 
            || (inst->opcode.opcode == 0x32) || (inst->opcode.opcode == 0x33)
            || (inst->opcode.opcode == 0x35) );
}

/* ALUB: SLL, SRL, SRA, MUL, MULI */
int isInALUB(Instruction *inst) {
    return ( ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x00) ) /* SLL */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x02) ) /* SRL */
            || ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.funct == 0x03) ) /* SRA */
            || ( (inst->opcode.opcode == 0x1c) && (inst->rtype.shamt == 0x00) && (inst->rtype.funct == 0x02) ) /* MUL */
            || ( inst->opcode.opcode == 0x21 )  /* MULI */
        );
}

/* MEM: LW, SW */
int isInMEM(Instruction *inst) {
    return ( (inst->itype.opcode == 0x23) || (inst->itype.opcode == 0x2b) );
}

int isNOP(Instruction *inst) {
    return ( (inst->opcode.opcode == 0x00) && (inst->rtype.rs == 0x00) && (inst->rtype.rt == 0x00) 
            && (inst->rtype.rd == 0x00) && (inst->rtype.funct == 0x00) );
}

int isBREAK(Instruction *inst) {
    return ( (inst->rtype.opcode == 0x00) && (inst->rtype.funct == 0x0d) );
}

int judgeInstNUll(Instruction *inst) {
    return ( (inst->opcode.opcode == 0) 
            && (inst->rtype.funct == 0) && (inst->rtype.shamt == 0) && (inst->rtype.rd == 0)
            && (inst->rtype.rt == 0) && (inst->rtype.rs == 0) && (inst->rtype.opcode == 0)
            && (inst->itype.offset == 0) && (inst->itype.rt == 0) && (inst->itype.rs == 0)
            && (inst->itype.opcode == 0) && (inst->jtype.imm == 0) && (inst->jtype.opcode == 0) );
}

int judgeInstREq(Instruction *inst1, Instruction *inst2) {
    return ( (inst1->opcode.opcode == inst2->opcode.opcode) 
        && (inst1->rtype.funct == inst2->rtype.funct) 
        && (inst1->rtype.rs == inst2->rtype.rs) 
        && (inst1->rtype.rt == inst2->rtype.rt)
        && (inst1->rtype.rd == inst2->rtype.rd) );
}

int judgeInstIEq(Instruction *inst1, Instruction *inst2) {
    return ( (inst1->opcode.opcode == inst2->opcode.opcode) 
        && (inst1->itype.offset == inst2->itype.offset) 
        && (inst1->itype.rs == inst2->itype.rs) 
        && (inst1->itype.rt == inst2->itype.rt) );
}

int judgeInstEq(Instruction *inst1, Instruction *inst2) {
    return ( judgeInstREq(inst1, inst2) || judgeInstIEq(inst1, inst2) );
}

/* This is sign extension. It adds 1 -> negative, 0 -> positive on leftside, and extend field to 32. */
/* field = The number needed to be extended. size = the length of field. */
int bitSigner(unsigned int field UNUSED, unsigned int size UNUSED) {
    int temp;
    if ((field >> (size - 1)) == 1) {                   /*Negative case.*/
        temp = field | (0xFFFFFFFF << size);
        return temp;
    } 
    else{
        return field;      /*Positive case.*/
    }
}

int get_imm_operand(Instruction inst UNUSED) {
    /*temp is the Imm[] before sign extension, i is a loop counter, len is the length of Imm, and Imm is the result.*/
    int temp = 0, len = 0, Imm = 0;

    /*Check opcode to judge the inst Type.*/
    switch(inst.opcode.opcode) {
        /*For R-Type, shamt 6...10.*/
        case(0x00): {
            temp = inst.rtype.shamt;
            len = 5;
            break;
        }

        /*For I-Type, 0...15.*/
        case 0x01:
        case 0x04:
        case 0x07:
        case 0x21:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x35:
        {
            temp = inst.itype.offset;
            len = 16;
            break;
        }

        /*For J-Type, 0...25.*/
        case(0x02): {   
            temp = inst.jtype.imm;
            len = 26;
            break;
        }

        /*Load and Store*/
        case 0x2b:
        case 0x23: {
            temp = inst.itype.offset;
            len = 16;
            break;
        }

        default: {
            handle_invalid_instruction(inst);
            printf("kkkk\n");
            exit(-1);
            /* return 0; */
        }
    }

    Imm = bitSigner(temp, len);              /*Run bitSigner to extend the result to 32 bits.*/

    return Imm;     /* The imm part in the inst in 32 bits.*/
}

/* the offsets return the offset in BYTES */

int get_jump_offset(Instruction inst UNUSED) {
    return (get_imm_operand(inst) << 2); 
}

int get_branch_offset(Instruction inst UNUSED) {
    return (get_imm_operand(inst) << 2);
}

int get_store_offset(Instruction inst UNUSED) {
    return get_imm_operand(inst); 
}

void handle_invalid_instruction(Instruction inst) {
    printf("Invalid Instruction: 0x%08x\n", inst.bits);
}

void handle_invalid_read(Address address) {
    printf("Bad Read. Address: 0x%08x\n", address);
    exit(-1);
}

void handle_invalid_write(Address address) {
    printf("Bad Write. Address: 0x%08x\n", address);
    exit(-1);
}
