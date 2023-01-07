SOURCES := utils.c part1.c part2.c part3.c main.c

HEADERS := types.h utils.h main.h

ASM_TESTS := sample single

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

%_disasm: mipscode/code/%.input mipscode/ref/%_disasm.solution MIPSsim
	@./MIPSsim -d $< > mipscode/out/disasm.output
	@diff -b -w -B $(word 2, $^) mipscode/out/disasm.output && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

# Part 2 Tests

part2: MIPSsim $(addsuffix _execute, $(ASM_TESTS))
	@echo "-----------Execute Tests Complete-----------"

%_execute: mipscode/code/%.input mipscode/ref/%_trace.solution MIPSsim
	@./MIPSsim -t $< > mipscode/out/trace.output
	@diff -b -w -B $(word 2, $^) mipscode/out/trace.output && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

# Part 3 Tests

part3: MIPSsim $(addsuffix _simulate, $(ASM_TESTS))
	@echo "-----------Simulation Tests Complete-----------"

%_simulate: mipscode/code/%.input mipscode/ref/%_score.solution MIPSsim
	@./MIPSsim -r $< > mipscode/out/score.output
	@diff -b $(word 2, $^) mipscode/out/score.output && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"


clean:
	rm -f MIPSsim
	rm -rf mipscode/out
