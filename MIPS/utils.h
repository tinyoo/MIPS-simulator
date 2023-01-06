#include "types.h"

#define RTYPE_FORMAT "%s\tR%d, R%d, R%d\n"
#define ITYPE_FORMAT "%s\tR%d, R%d, #%d\n"
#define MEM_FORMAT "%s\tR%d, %d(R%d)\n"
#define UTYPE_FORMAT "%s\tR%d, #%d\n"
#define J_FORMAT "J\t#%d\n"
#define JR_FORMAT "JR\tR%d\n"
#define BRANCH_FORMAT "%s\tR%d, R%d, %d\n"
#define BREAK_FORMAT "BREAK\n"
#define NOP_FORMAT "\n"

#define RTYPE_FORMAT2 "[%s\tR%d, R%d, R%d]\n"
#define ITYPE_FORMAT2 "[%s\tR%d, R%d, #%d]\n"
#define MEM_FORMAT2 "[%s\tR%d, %d(R%d)]\n"
#define UTYPE_FORMAT2 "[%s\tR%d, #%d]\n"
#define J_FORMAT2 "J\t#%d\n"
#define JR_FORMAT2 "JR\tR%d\n"
#define BRANCH_FORMAT2 "%s\tR%d, R%d, %d\n"
#define BREAK_FORMAT2 "BREAK\n"
#define NOP_FORMAT2 "\n"

/* Helper functions are helpful. */
int isBranch(Instruction *);
int isOPI(Instruction *);
int isR(Instruction *);
int isI(Instruction *);
int isShift(Instruction *);
int isJ(Instruction *);
int isInALU(Instruction *);
int isInALUB(Instruction *);
int isInMEM(Instruction *);
int isNOP(Instruction *);
int isBREAK(Instruction *);
int judgeInstNUll(Instruction *);
int judgeInstREq(Instruction *, Instruction *);
int judgeInstIEq(Instruction *, Instruction *);
int judgeInstEq(Instruction *, Instruction *);
int bitSigner(unsigned, unsigned);
int get_imm_operand(Instruction);
int get_jump_offset(Instruction);
int get_branch_offset(Instruction);
int get_store_offset(Instruction);

void handle_invalid_instruction(Instruction);
void handle_invalid_read(Address);
void handle_invalid_write(Address);
