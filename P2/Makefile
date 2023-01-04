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

%_disasm: mipscode/code/%.input mipscode/ref_disasm/%.solution MIPSsim
	@./MIPSsim -d $< > mipscode/out/test.dump
	@diff -b $(word 2, $^) mipscode/out/test.dump && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

# Part 3 Tests

part3: MIPSsim $(addsuffix _simulate, $(ASM_TESTS))
	@echo "-----------Simulation Tests Complete-----------"

%_simulate: mipscode/code/%.input mipscode/ref_sim/%.solution MIPSsim
	@./MIPSsim -r $< > mipscode/out/output.txt
	@diff -b $(word 2, $^) mipscode/out/output.txt && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"


clean:
	rm -f MIPSsim
	rm -rf mipscode/out
