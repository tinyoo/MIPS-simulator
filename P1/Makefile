SOURCES := utils.c part1.c part2.c mips.c
#SOURCES := utils.c part1.c mips.c
HEADERS := types.h utils.h mips.h

ASM_TESTS := sample

all: mips part1 part2
	@echo "=============All tests finished============="

.PHONY: part1 part2 %_disasm %_execute

mips: $(SOURCES) $(HEADERS) out
	gcc -g -Wpedantic -Wall -Wextra -Werror -std=c89 -o $@ $(SOURCES)

out:
	@mkdir -p ./mipscode/out

# Part 1 Tests

part1: mips $(addsuffix _disasm, $(ASM_TESTS))
	@echo "---------Disassembly Tests Complete---------"

%_disasm: mipscode/code/%.input mipscode/ref/%.solution mips
	@./mips -d $< > mipscode/out/test.dump
	@diff $(word 2, $^) mipscode/out/test.dump && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

# Part 2 Tests

part2: mips $(addsuffix _execute, $(ASM_TESTS))
	@echo "-----------Execute Tests Complete-----------"

%_execute: mipscode/code/%.input mipscode/ref/%.trace mips
	@./mips -r $< > mipscode/out/test.trace
	@diff $(word 2, $^) mipscode/out/test.trace && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

clean:
	rm -f mips
	rm -rf mipscode/out
