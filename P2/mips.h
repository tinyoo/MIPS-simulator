#ifndef MIPS_H
#define MIPS_H

#include "types.h"

/* see part1.c */
void toBin(int32_t n);
void decode_instruction(Instruction i, int index);

/* see part2.c */
void execute_instruction(Instruction instruction,Processor* processor,Byte *memory);
void store(Byte *memory,Address address,Alignment alignment,Word value, int);
Word load(Byte *memory,Address address,Alignment alignment, int);

#endif
