SOURCES := utils.c part1.c part2.c part3.c main.c

HEADERS := types.h utils.h main.h

ASM_TESTS := sample

all: MIPSsim part1 part2 part3
	@echo "=============All tests finished============="

.PHONY: part1 part2 part3 %_disasm %_simulate

MIPSsim: $(SOURCES) $(HEADERS) out
	gcc -g -Wpedantic -Wall -Wextra -Werror -std=c89 -o $@ $(SOURCES)

out:
	@mkdir -p ./mipscode/out

# Part 1 Tests

part1: MIPSsim $(addsuffix _disasm, $(ASM_TESTS))
	@echo "---------Disassembly Tests Complete---------"

%_disasm: mipscode/code/%.input mipscode/ref/%.disasm MIPSsim
	@./MIPSsim -d $< > mipscode/out/output.disasm
	@diff -b $(word 2, $^) mipscode/out/output.disasm && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

# Part 2 Tests

part2: MIPSsim $(addsuffix _execute, $(ASM_TESTS))
	@echo "-----------Execute Tests Complete-----------"

%_execute: mipscode/code/%.input mipscode/ref/%.trace MIPSsim
	@./MIPSsim -t $< > mipscode/out/output.trace
	@diff $(word 2, $^) mipscode/out/output.trace && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

# Part 3 Tests

part3: MIPSsim $(addsuffix _simulate, $(ASM_TESTS))
	@echo "-----------Simulation Tests Complete-----------"

%_simulate: mipscode/code/%.input mipscode/ref/%.score MIPSsim
	@./MIPSsim -r $< > mipscode/out/output.score
	@diff -b $(word 2, $^) mipscode/out/output.score && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"


clean:
	rm -f MIPSsim
	rm -rf mipscode/out
