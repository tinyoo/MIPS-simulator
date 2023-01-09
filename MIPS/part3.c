#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "utils.h"
#include "main.h"

#define MAX_SIZE 50

/*  post buffers: executed insts, next cycle shows the result, run exe in this cycle, record the Fi to be released, clear the post buffer  */
/*  pre queues:   insts to be executed  */
/*  Q to Post: 
		if Q[0] is SW, exe it, update Q, update the len after the whole cycle   
		if post buffer is empty, Q[0] -> post, update Q, update the len after the whole cycle */
/*  pre-issue buffer: the fetched insts into after scoreboarding, all Fis in rrs update to 1*/
/*  pIB to Q:  if Q has slot, inst's Fj, Fk in rrs is 0, send it to Q, update pIB, update the len after the whole cycle, update the Fi right now   */
/*  fetch:  inst1, inst2 fetch when waitinginst is empty
			if no place in pIB, pc += 0 (not fetch branch too)
			if one place in pIB, fetch 1 inst
				if inst is not branch, to pIB, update pIB, update len after the whole cycle
					PC += 4
				if inst is J, fill exeinst
					PC += 0
				if inst is branch(not J) fill waiting inst after the whole cycle
					stall until:  rrs[Fj, FK] become 0, exe branch, waitinst=>nop, exeinst=>branchinst
					PC += 0
			if two places in pIB, fetch 2 insts 
				if inst1, inst2 are not branches not break, to pIB, update pIB, update len after the whole cycle
					PC += 8       
				if inst1 is not branch break and inst2 is branch(not J),  
					inst1 to pIB, update pIB, update len after the whole cycle
					inst2 => waitinginst
					PC += 4
				if inst1 is not branch break and inst2 is J
					inst1 to pIB, inst2 fill exeinst
					PC += 0
				if inst1 is J,  fill exeinst right now
					PC += 0
				if inst1 not break, inst2 break, exe=>break
waitinst: if branch(not J), if rrs[Fj, Fk] all becomes 0, exeinst=>branchinst, waitinst=>nop, exe it
exeinst: if J, exe it and erase 

*/

/* tmpreg to store the recorded Fis of the executed inst in post buffers*/
unsigned int tmpreg1 = 33;
unsigned int tmpreg2 = 33;
unsigned int tmpreg3 = 33;

int cc = 0;
int jflag = 0;
Instruction tmpj;

Byte *memory;

int judgePrevLS(Instruction *, int);
void callBack(Instruction *, int *);
int judgeReady(Instruction *, Processor *);
int branchReady(Instruction *);
void setFi(Instruction *, Processor *);
int getFi(Instruction *);
int getFj(Instruction *);
int getFk(Instruction *);


/* exe post buffers */
void WB(Processor *p UNUSED) {
	execute_instruction(postALUBuffer, p, memory);
	p -> PC -= 4;
	tmpreg1 = getFi(&postALUBuffer);
	postALUBuffer = nop;

	execute_instruction(postALUBBuffer, p, memory);
	p -> PC -= 4;
	tmpreg2 = getFi(&postALUBBuffer);
	postALUBBuffer = nop;

	execute_instruction(postMEMBuffer, p, memory);
	p -> PC -= 4;
	tmpreg3 = getFi(&postMEMBuffer);
	postMEMBuffer = nop;
}

/* pre queues to post */
void Exe(Processor *p UNUSED) {
	int i;

	/* Q[0] is SW */
	if (pMQ.preMEMQueue[0].opcode.opcode == 0x2b) {}

	else {
		if (judgeInstNUll(&postALUBuffer) && !judgeInstNUll(&pALU.preALUQueue[0])) {
			postALUBuffer = pALU.preALUQueue[0];
			for (i = 0; i < pALU.len-1; ++i)
				pALU.preALUQueue[i] = pALU.preALUQueue[i+1];
			pALU.preALUQueue[pALU.len-1] = nop;
			pALU.tlen -= 1;
		}
		/* two cycles */
		if (judgeInstNUll(&postALUBBuffer) && !judgeInstNUll(&pALUB.preALUBQueue[0])) {
			cc += 1;
			if (cc == 2) {
				postALUBBuffer = pALUB.preALUBQueue[0];
				for (i = 0; i < pALUB.len-1; ++i)
					pALUB.preALUBQueue[i] = pALUB.preALUBQueue[i+1];
				pALUB.preALUBQueue[pALUB.len-1] = nop;
				pALUB.tlen -= 1;
				cc = 0;
			}
		}
		
		if (judgeInstNUll(&postMEMBuffer) && !judgeInstNUll(&pMQ.preMEMQueue[0])) {
			postMEMBuffer = pMQ.preMEMQueue[0];
			for (i = 0; i < pMQ.len-1; ++i)
				pMQ.preMEMQueue[i] = pMQ.preMEMQueue[i+1];
			pMQ.preMEMQueue[pMQ.len-1] = nop;
			pMQ.tlen -= 1;
		}
	}
}

/* pIB to pre queues */
void Issue(Processor *p UNUSED) {   /* PIB -> Queue */
	int i = 0, j = 0, pt = 0;
	int len = pIB.tlen;
	int tm = pMQ.len;
	/*int flag = 1;*/
	Instruction inst;

	/* Q[0] is SW */
	if (pMQ.preMEMQueue[0].opcode.opcode == 0x2b) {
		execute_instruction(pMQ.preMEMQueue[0], p, memory);
		p->PC -= 4;
		for (i = 0; i < pMQ.len-1; ++i)
			pMQ.preMEMQueue[i] = pMQ.preMEMQueue[i+1];
		pMQ.preMEMQueue[pMQ.len-1] = nop;
		pMQ.tlen -= 1;
		tm = 0;
	}
	/*printf("%d\n", tm);*/
	
	while (i < len) {
		inst = pIB.preIssueBuffer[pt];
		/*
		flag = 1;
		if ( !(isInMEM(&inst)) && judgePrevLS(&inst, i))
			flag = 0;
		*/
		
		/*printf("%d\n", rrs[inst.itype.rt]);*/
		if (judgeReady(&inst, p) /*&& (flag == 1)*/) {
			if ( isInALU(&inst) && (pALU.len < 2) ) {
				pALU.preALUQueue[pALU.tlen] = inst;
				pALU.tlen += 1;
				for (j = pt; j < pIB.tlen-1; ++j) {
					pIB.preIssueBuffer[j] = pIB.preIssueBuffer[j+1];
				}
				pIB.preIssueBuffer[pIB.tlen-1] = nop;
				pIB.tlen -= 1;
				setFi(&inst, p);
			}

			else if (isInALUB(&inst) && (pALUB.len < 2) ) {
				pALUB.preALUBQueue[pALUB.tlen] = inst;
				pALUB.tlen += 1;
				for (j = pt; j < pIB.tlen-1; ++j) {
					pIB.preIssueBuffer[j] = pIB.preIssueBuffer[j+1];
				}
				pIB.preIssueBuffer[pIB.tlen-1] = nop;
				pIB.tlen -= 1;
				setFi(&inst, p);
			}

			else if (isInMEM(&inst) && (pMQ.len < 2) ) {
				pMQ.preMEMQueue[pMQ.tlen] = inst;
				pMQ.tlen += 1;
				/* if preMQ is empty in previous cyle, push into 2*/
				/* if preMQ has one filled in precious cycle, push into 1 */
				if (tm == 0)
					pMQ.len += 1;
				for (j = pt; j < pIB.tlen-1; ++j) {
					pIB.preIssueBuffer[j] = pIB.preIssueBuffer[j+1];
				}
				pIB.preIssueBuffer[pIB.tlen-1] = nop;
				pIB.tlen -= 1;
				setFi(&inst, p);
			}
		}
		else if (!judgeReady(&inst, p)){
			if (inst.opcode.opcode != 0x23)
				setFi(&inst, p);
			pt += 1;
		}
		i++;
	}
}

int judgePrevLS(Instruction *inst, int j) {
	int i = 0;
	int flag = 0;
	if ((inst->opcode.opcode != 0x2b) && (inst->opcode.opcode != 0x23)) {
		for (i = 0; i < j; ++i) {
			if (pIB.preIssueBuffer[i].opcode.opcode == 0x2b) {
				if (getFi(&pIB.preIssueBuffer[i]) == getFi(inst))
					flag += 1;
			}
			if (pIB.preIssueBuffer[i].opcode.opcode == 0x23) {
				if (getFi(&pIB.preIssueBuffer[i]) == getFi(inst) ||
					getFj(&pIB.preIssueBuffer[i]) == getFi(inst) ||
					getFk(&pIB.preIssueBuffer[i]) == getFi(inst))
					flag += 1;
			}
		}
	}
	return (flag != 0);
}

/* fetch to fill pIB */
void IF(Instruction inst1, Instruction inst2, Processor *p UNUSED) {
	if (judgeInstNUll(&waitingInst)) {
		/* pIB full */
		if ( pIB.len == 4 )
			p -> PC += 0;

		/* pIB has one slot */
		else if ( pIB.len == 3 ) {
			if (isNOP(&inst1))	{p -> PC += 4; }
			else if ( !isBREAK(&inst1) && !isBranch(&inst1) && !isJ(&inst1) ) {
				pIB.preIssueBuffer[pIB.tlen] = inst1;
				pIB.tlen += 1;
				p -> PC += 4;
			}
			else if (isJ(&inst1)) {
				jflag = 1;
				execute_instruction(inst1, p, memory);
				tmpj = inst1;
				p -> PC += 0;
			}
			else if (isBranch(&inst1)) {
				waitingInst = inst1;
			}
		}

		/* pIB has more than 2 slots */
		else if (pIB.len <= 2) {
			if (isNOP(&inst1) && isNOP(&inst2)) {p -> PC += 8; }
			else if ( !isBREAK(&inst1) && !isBranch(&inst1) && !isJ(&inst1) && !isNOP(&inst1)
				&& !isBREAK(&inst2) && !isBranch(&inst2) && !isJ(&inst2) && !isNOP(&inst2) ) {
				p -> PC += 8;
				pIB.preIssueBuffer[pIB.tlen] = inst1;
				pIB.preIssueBuffer[pIB.tlen+1] = inst2;
				pIB.tlen += 2;
			}
			else if ( !isBREAK(&inst1) && !isBranch(&inst1) && !isJ(&inst1) && !isNOP(&inst1) && isBranch(&inst2) ) {
				p -> PC += 4;
				pIB.preIssueBuffer[pIB.tlen] = inst1;
				pIB.tlen += 1;
				waitingInst = inst2;
			}
			else if ( !isBREAK(&inst1) && !isBranch(&inst1) && !isJ(&inst1) && !isNOP(&inst1) && isJ(&inst2) ) {
				p -> PC += 0;
				pIB.preIssueBuffer[pIB.tlen] = inst1;
				pIB.tlen += 1;
				jflag = 1;
				execute_instruction(inst2, p, memory);
				tmpj = inst2;
			}
			else if (isJ(&inst1)) {
				jflag = 1;
				execute_instruction(inst1, p, memory);
				tmpj = inst1;
				p -> PC += 0;
			}
			else if (isBranch(&inst1)) {
				waitingInst = inst1;
			}
			else if (!isBREAK(&inst1) && isBREAK(&inst2)) {
				jflag = 1;
				pIB.preIssueBuffer[pIB.tlen] = inst1;
				execute_instruction(inst2, p, memory);
				tmpj = inst2;
				p -> PC += 0;
			}
			else if (isBREAK(&inst1)) {
				if (allEmpty()) {
					jflag = 1;
					execute_instruction(inst1, p, memory);
					tmpj = inst1;
					p -> PC += 0;
				}
				else {
					waitingInst = inst1;
				}
			}
		}
	}
}

int allEmpty() {
	return (pIB.len == 0 && pALU.len == 0 && pALUB.len == 0 && isNOP(&postALUBuffer) && isNOP(&postMEMBuffer) && isNOP(&postALUBBuffer));
}

void updateExe(Processor *p UNUSED) {
	if (isJ(&executedInst) && (jflag == 0)) {
		
		executedInst = nop;
	}
	if (jflag == 1) {
		executedInst = tmpj;
		jflag = 0;
	}
	if (isBranch(&executedInst))
		executedInst = nop;
}

void updateWaiting(Processor *p UNUSED) {
	if (isBranch(&waitingInst)) {
		if (judgeReady(&waitingInst, p) && branchReady(&waitingInst)) {
			executedInst = waitingInst;
			waitingInst = nop;
			execute_instruction(executedInst, p, memory);
		}
	}
	else if (isBREAK(&waitingInst)) {
		if (allEmpty()) {
			jflag = 1;
			executedInst = waitingInst;
			waitingInst = nop;
			execute_instruction(waitingInst, p, memory);
			tmpj = waitingInst;
		}
	}
}

void update() {
	pIB.len = pIB.tlen;
	pALU.len = pALU.tlen;
	pALUB.len = pALUB.tlen;
	pMQ.len = pMQ.tlen;
	rrs[tmpreg1] = 0;
	rrs[tmpreg2] = 0;
	rrs[tmpreg3] = 0;
	jflag = 0;
}

void callBack(Instruction *inst, int *flag) {
	int i;
	for (i = 0; i < pIB.len; ++i) {
		if (judgeInstEq(&pIB.preIssueBuffer[i], inst))
			break;
		if ( (getFi(&pIB.preIssueBuffer[i]) == getFj(inst)) 
			|| (getFi(&pIB.preIssueBuffer[i]) == getFk(inst)) )
			*flag += 1;
		if (pIB.preIssueBuffer[i].opcode.opcode == 0x2b || pIB.preIssueBuffer[i].opcode.opcode == 0x23) {
			if (getFi(&pIB.preIssueBuffer[i]) == getFi(inst))
				*flag += 1;
		}
	}
	for (i = 0; i < pALU.len; ++i) {
		if ( (getFi(&pALU.preALUQueue[i]) == getFj(inst)) 
			|| (getFi(&pALU.preALUQueue[i]) == getFk(inst)) )
			*flag += 1;
	}
	if ( (getFi(&postALUBuffer) == getFj(inst)) || (getFi(&postALUBuffer) == getFk(inst)) )
		*flag += 1;

	for (i = 0; i < pALUB.len; ++i) {
		if ( (getFi(&pALUB.preALUBQueue[i]) == getFj(inst)) 
			|| (getFi(&pALUB.preALUBQueue[i]) == getFk(inst)) )
			*flag += 1;
	}
	if ( (getFi(&postALUBBuffer) == getFj(inst)) || (getFi(&postALUBBuffer) == getFk(inst)) )
		*flag += 1;
	
	for (i = 0; i < pMQ.len; ++i) {
		if ( (getFi(&pMQ.preMEMQueue[i]) == getFj(inst)) 
			|| (getFi(&pMQ.preMEMQueue[i]) == getFk(inst)) )
			*flag += 1;
	}
	if ( (getFi(&postMEMBuffer) == getFj(inst)) || (getFi(&postMEMBuffer) == getFk(inst)) )
		*flag += 1;
}

int branchReady(Instruction *inst) {
	int i;
	int ans = 1;
	if ( ( (inst->opcode.opcode == 0x01)  && (inst->itype.rt == 0x00) ) /* BLTZ*/
            || ( (inst->opcode.opcode == 0x07)  && (inst->itype.rt == 0x00) ) /* BGTZ */ ) {
		for (i = 0; i < pIB.len; ++i) {
			if (pIB.preIssueBuffer[i].opcode.opcode == 0x23) {
				if ( pIB.preIssueBuffer[i].itype.rt == inst->itype.rs)
					ans = 0;
			}
		}
		for (i = 0; i < pMQ.len; ++i) {
			if (pMQ.preMEMQueue[i].opcode.opcode == 0x23) {
				if ( pMQ.preMEMQueue[i].itype.rt == inst->itype.rs)
					ans = 0;
			}
		}
	}
	else if ( inst->opcode.opcode == 0x04 ) /* BEQ */ {
		for (i = 0; i < pIB.len; ++i) {
			if (pIB.preIssueBuffer[i].opcode.opcode == 0x23) {
				if ( (pIB.preIssueBuffer[i].itype.rt == inst->itype.rs) 
					|| (pIB.preIssueBuffer[i].itype.rt == inst->itype.rt) )
					ans = 0;
			}
		}
		for (i = 0; i < pMQ.len; ++i) {
			if (pMQ.preMEMQueue[i].opcode.opcode == 0x23) {
				if ( ( pMQ.preMEMQueue[i].itype.rt == inst->itype.rs) 
					|| ( pMQ.preMEMQueue[i].itype.rt == inst->itype.rt) )
					ans = 0;
			}
		}
	}
	return (ans == 1);
}

int judgeReady(Instruction *inst, Processor *p UNUSED) {
	int flag = 0;

	if ( (inst->opcode.opcode != 0x2b) && (inst->opcode.opcode != 0x23)) {
		callBack(inst, &flag);
		if (flag == 0)
			rrs[getFi(inst)] = 0;
		else
			rrs[getFi(inst)] = 1;
	}

	if (isR(inst) && !isBREAK(inst) && !isShift(inst) && 
		!( (inst->opcode.opcode == 0x00) && (inst->rtype.funct == 0x08) ))
		return ( (rrs[inst->rtype.rs] == 0) && (rrs[inst->rtype.rt] == 0) );
	else if (isShift(inst))
		return ( rrs[inst->rtype.rt] == 0 );
	else if (inst->opcode.opcode == 0x23)  /* LW */ {
		return ( (rrs[inst->itype.rs] == 0) && (rrs[inst->itype.rt] == 0));
	}
	else if (inst->opcode.opcode == 0x2b)  /* SW */ {
		return ( (rrs[inst->itype.rs] == 0) && (rrs[inst->itype.rt] == 0) );
	}
	else if (isOPI(inst))
		return ( rrs[inst->itype.rs] == 0 );
	else if (isBranch(inst))
		return (rrs[inst->itype.rs] == 0);
	else
		return 0;  /*error*/
}


void setFi(Instruction *inst, Processor *p UNUSED) {
	if (isR(inst) && !isBREAK(inst) && 
		!( (inst->opcode.opcode == 0x00) && (inst->rtype.funct == 0x08) ))
		rrs[inst->rtype.rd] = 1;
	else if (inst->opcode.opcode == 0x23)  /* LW */
		rrs[inst->itype.rt] = 1;
	else if (isOPI(inst))
		rrs[inst->itype.rt] = 1;
}

int getFi(Instruction *inst) {
	if (isR(inst) && !isBREAK(inst) && 
		!( (inst->opcode.opcode == 0x00) && (inst->rtype.funct == 0x08) ))
		return inst->rtype.rd;
	else if (inst->opcode.opcode == 0x23)  /* LW */
		return inst->itype.rt;
	else if (inst->opcode.opcode == 0x2b) /* SW */
		return null;
	else if (isOPI(inst))
		return inst->itype.rt;
	else
		return null;
}

int getFj(Instruction *inst) {
	if (isR(inst) && !isBREAK(inst) && !isShift(inst) &&
		!( (inst->opcode.opcode == 0x00) && (inst->rtype.funct == 0x08) ))
		return inst->rtype.rs;
	else if (isShift(inst))
		return null;
	else if (inst->opcode.opcode == 0x23)  /* LW */
		return inst->itype.rs;
	else if (isOPI(inst))
		return inst->itype.rs;
	else if (isBranch(inst)) {
		if ( ( inst->opcode.opcode == 0x04 ) /* BEQ */
			||( (inst->opcode.opcode == 0x01)  && (inst->itype.rt == 0x00) ) /* BLTZ*/
            || ( (inst->opcode.opcode == 0x07)  && (inst->itype.rt == 0x00) ) /* BGTZ */ )
			return inst->itype.rs;
		else
			return null;
	}
	else
		return null;
}

int getFk(Instruction *inst) {
	if (isR(inst) && !isBREAK(inst) && !isShift(inst) &&
		!( (inst->opcode.opcode == 0x00) && (inst->rtype.funct == 0x08) ))
		return inst->rtype.rt;
	else if (isShift(inst))
		return inst->rtype.rt;
	else if (inst->opcode.opcode == 0x23)  /* LW */
		return inst->itype.rt;
	else if (isOPI(inst))
		return inst->itype.rt;
	else if (isBranch(inst)) {
		if ( ( (inst->opcode.opcode == 0x01)  && (inst->itype.rt == 0x00) ) /* BLTZ*/
            || ( (inst->opcode.opcode == 0x07)  && (inst->itype.rt == 0x00) ) /* BGTZ */ )
			return null;
		else if ( inst->opcode.opcode == 0x04 ) /* BEQ */
			return inst->itype.rt;
		else
			return null;
	}
	else
		return 0;
}
