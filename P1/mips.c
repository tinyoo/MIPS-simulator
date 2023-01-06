#include "mips.h"
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


void execute(Processor *processor, int prompt, int print) {
    Instruction instruction; 
    int startaddr;
    int tmpp = 0;
    int flag = 0;

    startaddr = 64;
    
    /* fetch an instruction */
    instruction.bits = load(memory, processor->PC, LENGTH_WORD, 1);

    /* interactive-mode prompt */
    if(prompt) {
        if(prompt == 1) {
            printf("simulator paused, enter to continue...");
            while(getchar() != '\n');
        }
        printf("%08x: ", processor->PC);
        decode_instruction(instruction,2);
    }

    if (instruction.rtype.funct == 0x0d)
        flag = 1;

    tmpp = processor->PC;
    
    execute_instruction(instruction, processor, memory); 

    /* enforce $0 being hard-wired to 0 */
    processor->R[0] = 0;
    
    /* print trace */
    if(print) {
        int i,k;
        cycle += 1;
        printf("--------------------\n");
        printf("Cycle:%d\t%d\t", cycle, tmpp);
        decode_instruction(instruction,2);    /*  */
        printf("\nRegisters\n");
        printf("R00:\t");
        for(i=0; i<15; i++) {
            printf("%d\t", processor->R[i]);
        }
        printf("%d\n", processor->R[15]);
        printf("R16:\t");
        for(i=16; i<31; i++) {
            printf("%d\t", processor->R[i]);
        }
        printf("%d\n", processor->R[31]);
        printf("\n");

        printf("Data\n");
        for(k=endaddr+4; k<4*countline+startaddr; k+=(8*4)) {
            printf("%d:\t", k);
            for (i=0; i<7; ++i) {
                printf("%d\t", bitSigner(load(memory, k+i*4, LENGTH_BYTE, 0), 8));
            }
            printf("%d\n", bitSigner(load(memory, k+7*4, LENGTH_BYTE, 0), 8));
        }
        printf("\n"); 
    }
    if (flag == 1)
        exit(0);
} 

void toBin(int32_t n) {
    int i = 0, a[6];
    for ( ; n > 0; n /= 2)
        a[i++] = n % 2;
    for (i = 5; i >= 0; i--)
        printf("%d", a[i]);
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
        if ( ((instruction & 0xFF) == 0x0d) && (((instruction >> 24) & 0xFF) == 0x00)  ) {
            endflag = 1;
            endaddr = startaddr + offset;
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
                printf("%c%c%c%c%c%c ", line[0],  line[1],  line[2],  line[3],  line[4], line[5]);
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
    int opt_disasm = 0, opt_regdump = 0, opt_interactive = 0, i;
    
    /* the architectural state of the CPU */
    Processor processor;
    
    /* parse the command-line args */
    int c;
    while((c=getopt(argc, argv, "drit")) != -1) {
        switch (c){
            case 'd':
                opt_disasm = 1;
                break;
             case 'r':
                opt_regdump = 1;
                break;
            case 'i':
                opt_interactive = 1;
                break;
            case 't':
                opt_interactive = 2;
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
        execute(&processor, opt_interactive, opt_regdump);
    }
    
    return 0;
}
