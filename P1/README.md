# User Manul

The input files should be in the form of xxx.input (e.g. sample.input) and be put into the version0/mipscode/code directory.
The reference output disassembly files should be in the form of xxx.solution (e.g. sample.solution) and be put into the version0/mipscode/ref directory, sharing the same name with input files.
The reference output trace files are in the form of xxx.trace (e.g. sample.trace) in the version0/mipscode/ref directory, sharing the same name with input files.

Run "make" in the version0 directory, and the output disassembly file "test.dump" and output trace file "test.trace" will be put into version0/mipscode/out directory.
.
├── Makefile
├── README.md
├── part1.c
├── part2.c
├── mips.c
├── mips.h
├── mips
├── mipscode
│   ├── code
│   │   └── sample.input
│   └── ref
│       ├── sample.solution
│       └── sample.trace
├── types.h
├── utils.c
└── utils.h
