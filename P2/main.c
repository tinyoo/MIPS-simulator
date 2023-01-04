#include "main.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>

#define MAX_SIZE 50


/* Pointer to simulator memory */
Byte *memory;

int endaddr;
int countline = 0;
int countexeline = 1;
int cycle = 0;
int endflag = 0;


void initialize() {
    int i;

    nop.opcode.opcode = 0;
    nop.rtype.funct = 0;
    nop.rtype.shamt = 0;
    nop.rtype.rd = 0;
    nop.rtype.rt = 0;
    nop.rtype.rs = 0;
    nop.rtype.opcode = 0;
    nop.itype.offset = 0;
    nop.itype.rt = 0;
    nop.itype.rs = 0;
    nop.itype.opcode = 0;
    nop.jtype.imm = 0;
    nop.jtype.opcode = 0;

    waitingInst = nop;
    executedInst = nop;

    pIB.preIssueBuffer[0] = nop;
    pIB.preIssueBuffer[1] = nop;
    pIB.preIssueBuffer[2] = nop;
    pIB.preIssueBuffer[3] = nop;
    pIB.len = 0;
    pIB.tlen = 0;

    pALU.preALUQueue[0] = nop;
    pALU.preALUQueue[1] = nop;
    pALU.len = 0;
    pALU.tlen = 0;

    postALUBuffer = nop;

    pALUB.preALUBQueue[0] = nop;
    pALUB.preALUBQueue[1] = nop;
    pALUB.len = 0;
    pALUB.tlen = 0;

    postALUBBuffer = nop;

    pMQ.preMEMQueue[0] = nop;
    pMQ.preMEMQueue[1] = nop;
    pMQ.len = 0;
    pMQ.tlen = 0;

    postMEMBuffer = nop;

    for (i = 0; i < 33; ++i)
        rrs[i] = 0;

    tmpinst = nop;
}


void simulate(Processor *processor, int print) {
    Instruction inst1, inst2; 
    int startaddr;
    int flag = 0;

    startaddr = 64;
    
    /* fetch 2 instructions */
    inst1.bits = load(memory, processor->PC, LENGTH_WORD, 1);
    inst2.bits = load(memory, processor->PC+4, LENGTH_WORD, 1);

    if (isBREAK(&inst1))
        flag = 1;

    /* enforce $0 being hard-wired to 0 */
    processor->R[0] = 0;

    printf("--------------------\n");
    cycle += 1;
    printf("Cycle:%d\n", cycle);
    printf("\n");

    WB(processor);
    Exe(processor);
    Issue(processor);
    IF(inst1, inst2, processor);
    updateExe(processor);
    updateWaiting(processor);
    update();

    /******************IF Unit******************/
    printf("IF Unit:\n");

    printf("\tWaiting Instruction: ");
    decode_instruction(waitingInst, 1);
    printf("\tExecuted Instruction: ");
    decode_instruction(executedInst, 1);

    /******************Pre-Issue Buffer******************/
    printf("Pre-Issue Buffer:\n");

    printf("\tEntry 0:");
    decode_instruction(pIB.preIssueBuffer[0], 2);
    printf("\tEntry 1:");
    decode_instruction(pIB.preIssueBuffer[1], 2);
    printf("\tEntry 2:");
    decode_instruction(pIB.preIssueBuffer[2], 2);
    printf("\tEntry 3:");
    decode_instruction(pIB.preIssueBuffer[3], 2);

    /******************Pre-ALU Queue******************/
    printf("Pre-ALU Queue:\n");
    printf("\tEntry 0:");
    decode_instruction(pALU.preALUQueue[0], 2);
    printf("\tEntry 1:");
    decode_instruction(pALU.preALUQueue[1], 2);

    /******************Post-ALU Buffer******************/
    printf("Post-ALU Buffer:");
    decode_instruction(postALUBuffer, 2);

    /******************Pre-ALUB Queue******************/
    printf("Pre-ALUB Queue:\n");
    printf("\tEntry 0:");
    decode_instruction(pALUB.preALUBQueue[0], 2);
    printf("\tEntry 1:");
    decode_instruction(pALUB.preALUBQueue[1], 2);

    /******************Post-ALUB Buffer******************/
    printf("Post-ALUB Buffer:");
    decode_instruction(postALUBBuffer, 2);

    /******************Pre-MEM Queue******************/
    printf("Pre-MEM Queue:\n");
    printf("\tEntry 0:");
    decode_instruction(pMQ.preMEMQueue[0], 2);
    printf("\tEntry 1:");
    decode_instruction(pMQ.preMEMQueue[1], 2);

    /******************Post-MEM Buffer******************/
    printf("Post-MEM Buffer:");
    decode_instruction(postMEMBuffer, 2);
    
    
    if(print) {
        int i,k;
        
        printf("\nRegisters\n");
        printf("R00:\t");
        for(i=0; i<7; i++) {
            printf("%d\t", processor->R[i]);
        }
        printf("%d\n", processor->R[7]);
        printf("R08:\t");
        for(i=8; i<15; i++) {
            printf("%d\t", processor->R[i]);
        }
        printf("%d\n", processor->R[15]);
        printf("R16:\t");
        for(i=16; i<23; i++) {
            printf("%d\t", processor->R[i]);
        }
        printf("%d\n", processor->R[23]);
        printf("R24:\t");
        for(i=24; i<31; i++) {
            printf("%d\t", processor->R[i]);
        }
        printf("%d\n", processor->R[31]);
        printf("\n");

        printf("Data\n");
        for(k=endaddr+4; k<4*countline+startaddr; k+=(8*4)) {
            printf("%d:\t", k);
            for (i=0; i<7; ++i) {
                printf("%d\t", bitSigner(load(memory, k+i*4, LENGTH_WORD, 0), 16));
            }
            printf("%d\n", bitSigner(load(memory, k+7*4, LENGTH_WORD, 0), 16));
        }
        /*printf("\n"); */
    }
    if (isBREAK(&inst2))
        flag = 1;

    if (flag == 1)
        exit(0);

} 


void load_program(uint8_t *mem, size_t memsize UNUSED, int startaddr, const char *filename, int disasm) {
    FILE *file = fopen(filename, "r");
    char line[MAX_SIZE];
    int instruction, offset = 0;

    while(fgets(line, MAX_SIZE, file) != NULL) {
        countline += 1;
        instruction = (int32_t) strtol(line, NULL, 2);
        mem[startaddr + offset] = instruction & 0xFF;
        mem[startaddr + offset + 1] = (instruction >> 8) & 0xFF;
        mem[startaddr + offset + 2] = (instruction >> 16) & 0xFF;
        mem[startaddr + offset + 3] = (instruction >> 24) & 0xFF;
        if ( ((instruction & 0xFF) == 0x0d) && (((instruction >> 24) & 0xFF) == 0x00) ) {
            endflag = 1;
            endaddr = startaddr + offset;
            /* printf("%d\n", endflag);
            printf("%d\n", startaddr + offset); */
        }
        if ( endflag == 0  ) {
            countexeline += 1;
        }
        if ((endflag == 1) & ((instruction & 0xFF) != 0x0d) ) {
            if (disasm) {
                line[32] = '\0';
                printf("%s\t", line);
                printf("%d\t", startaddr + offset);
                printf("%d\n", instruction);
            }
        }
        else {
            if (disasm){
                uint8_t *inst_ptr = (uint8_t *) &instruction;
                printf("%c %c%c%c%c%c ", line[0],  line[1],  line[2],  line[3],  line[4], line[5]);
                printf("%c%c%c%c%c ",   line[6],  line[7],  line[8],  line[9],  line[10]);
                printf("%c%c%c%c%c ",   line[11], line[12], line[13], line[14], line[15]);
                printf("%c%c%c%c%c ",   line[16], line[17], line[18], line[19], line[20]);
                printf("%c%c%c%c%c ",   line[21], line[22], line[23], line[24], line[25]);
                printf("%c%c%c%c%c%c\t",line[26], line[27], line[28], line[29], line[30], line[31]);
                printf("%d\t", startaddr + offset);
                decode_instruction(*((Instruction *) inst_ptr),1);
            }
        }
        offset += 4;
    } 
}

int main(int argc,char** argv) {
    /* options */
    int opt_disasm = 0, opt_regdump = 0, i;

    int c;
    
    /* the architectural state of the CPU */
    Processor processor;

    initialize();
    
    /* parse the command-line args */
    while((c=getopt(argc, argv, "drit")) != -1) {
        switch (c){
            case 'd':
                opt_disasm = 1;
                break;
             case 'r':
                opt_regdump = 1;
                break; 
            default:
                fprintf(stderr, "Bad option %c\n",c);
                return -1;
        }
    }
    
    /* make sure we got an executable filename on the command line */
    if(argc <= optind) {
        fprintf(stderr, "Give me an executable file to run!\n");
        return -1;
    }

    /* load the executable into memory */
    assert(memory == NULL);
    memory = calloc(MEMORY_SPACE, sizeof(uint8_t)); /* allocate zeroed memory */
    assert(memory != NULL);
  
    /* Set the PC to 64 */ 
    processor.PC = 64;
    load_program(memory, MEMORY_SPACE, processor.PC, argv[optind], opt_disasm);
    
    /* if we're just disassembling, exit here */
    if(opt_disasm) {
        return 0;
    }
    
    /* initialize the CPU */
    /* zero out all registers */
    for(i=0; i<32; i++) {
        processor.R[i] = 0;
    }
 
    /* printf("%d", countexeline); */
    /* simulate forever! */
    for(;;) {
        simulate(&processor, opt_regdump);
    }
    
    return 0;
}
