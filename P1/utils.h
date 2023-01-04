#include "types.h"

#define RTYPE_FORMAT "%s R%d, R%d, R%d\n"
#define ITYPE_FORMAT "%s R%d, R%d, #%d\n"
#define MEM_FORMAT "%s R%d, %d(R%d)\n"
#define UTYPE_FORMAT "%s R%d, #%d\n"
#define J_FORMAT "J #%d\n"
#define JR_FORMAT "JR R%d\n"
#define BRANCH_FORMAT "%s R%d, R%d, %d\n"
#define BREAK_FORMAT "BREAK\n"
#define NOP_FORMAT "NOP\n"

#define RTYPE_FORMAT2 "%s\tR%d, R%d, R%d\n"
#define ITYPE_FORMAT2 "%s\tR%d, R%d, #%d\n"
#define MEM_FORMAT2 "%s\tR%d, %d(R%d)\n"
#define UTYPE_FORMAT2 "%s\tR%d, #%d\n"
#define J_FORMAT2 "J\t#%d\n"
#define JR_FORMAT2 "JR\tR%d\n"
#define BRANCH_FORMAT2 "%s\tR%d, R%d, %d\n"
#define BREAK_FORMAT2 "BREAK\n"
#define NOP_FORMAT2 "NOP\n"

/* Helper functions are helpful. */
int bitSigner(unsigned, unsigned);
int get_imm_operand(Instruction);
int get_jump_offset(Instruction);
int get_branch_offset(Instruction);
int get_store_offset(Instruction);

void handle_invalid_instruction(Instruction);
void handle_invalid_read(Address);
void handle_invalid_write(Address);
