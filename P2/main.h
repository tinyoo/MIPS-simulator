#ifndef MAIN_H
#define MAIN_H

#include "types.h"

#define null 32

Instruction waitingInst;
Instruction executedInst;
Instruction nop;
Instruction tmpinst;

struct preIssue {
	Instruction preIssueBuffer[4];
	int len;
	int tlen;
} pIB;

struct preALU {
	Instruction preALUQueue[2];
	int len;
	int tlen;
} pALU;

Instruction postALUBuffer;

struct preALUB {
	Instruction preALUBQueue[2];
	int len;
	int tlen;
} pALUB;

Instruction postALUBBuffer;

struct preMEM {
	Instruction preMEMQueue[2];
	int len;
	int tlen;
} pMQ;

Instruction postMEMBuffer;

int rrs[33];

/* see part1.c */
void decode_instruction(Instruction i, int index);

/* see part2.c */
void execute_instruction(Instruction instruction,Processor* processor,Byte *memory);
void store(Byte *memory,Address address,Alignment alignment,Word value, int);
Word load(Byte *memory,Address address,Alignment alignment, int);

/* see part3.c */
void callBack(Instruction *, int *);
int judgeReady(Instruction *, Processor *);
int branchReady(Instruction *);
void setFi(Instruction *, Processor *);
int getFi(Instruction *);
int getFj(Instruction *);
int getFk(Instruction *);
void IF(Instruction, Instruction, Processor *);
void Issue(Processor *);
void Exe(Processor *);
void WB(Processor *);
void update();
void updateWaiting(Processor *);
void updateExe(Processor *);

#endif
