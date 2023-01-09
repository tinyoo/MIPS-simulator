# User Manul

## Run
make
./MIPSsim sample.txt -r &> output.txt
diff -b output.txt simulation.txt

## make
You can using the "make" command to get a output fime "output.txt" in "mipscode/out" directory, and the comparison between the output file and the reference file is done at the same time:

The input file should be in the form of xxx.input (e.g. sample.input) and be put into the mipscode/code directory.

The reference files should be in the form of xxx.solution and be put into the mipscode/ref directory. For "sample.input", the disassembly reference should be "sample_disasm.solution", the single-cycle processor simulation reference should be "sample_trace.solution", and the pipelined processor simulation reference should be "sample_score.solution".

The output trace file (e.g. sample.output) will be put in the /mipscode/out directory.

Comparison: "sample_simulate TEST PASSED!" means the output pipelined processor simulation file is the same with the ref.

## ./MIPSsim sample.txt -r &> output.txt
-d: disassembly
-t: non-pipelined processor simulation
-r: pipelined processor simulation

sample.txt is the input and should be put in the root directory.
output.txt is the output file, and it will be placed in the root directory.
The file simulation.txt is the reference, and you can run "diff -b output.txt simulation.txt" to compare the output and the ref.

.
├── Makefile
├── README.md
├── part1.c
├── part2.c
├── part3.c
├── mips.c
├── mips.h
├── main.c
├── main.h
├── MIPSsim
├── mipscode
│   ├── code
│   │   └── sample.input
│	├── out
│   │   │── disasm.output
│   │   │── score.output
│   │   └── trace.output
│   └── ref
│       │── sample_disasm.solution
│       │── sample_score.solution
│       └── sample_trace.solution
├── sample.txt
├── output.txt
├── simulation.txt
├── types.h
├── utils.c
└── utils.h
