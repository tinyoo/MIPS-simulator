# User Manul

## Run
make
./MIPSsim sample.txt -r &> output.txt
diff -b output.txt simulation.txt

## make
You can using the "make" command to get a output fime "output.txt" in "mipscode/out" directory, and the comparison between the output file and the reference file is done at the same time:

The input file should be in the form of xxx.input (e.g. sample.input) and be put into the mipscode/code directory.

The reference simulation file should be in the form of xxx.solution (e.g. sample.solution) and be put into the mipscode/ref_sim directory, sharing the same name with input file.

The output trace file output.txt will be put in the /mipscode/out directory.

Comparison: "sample_simulate TEST PASSED!" means the output file is the same with the ref.

## ./MIPSsim sample.txt -r &> output.txt
You can using the command "./MIPSsim sample.txt -r &> output.txt" (-r means outputing a simulating trace, and -d means outputing the disassembly file):
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
│   │   │── output.disasm
│   │   │── output.score
│   │   └── output.trace
│   └── ref
│       │── sample.disasm
│       │── sample.score
│       └── sample.trace
├── sample.txt
├── output.txt
├── simulation.txt
├── types.h
├── utils.c
└── utils.h
