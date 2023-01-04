#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


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
