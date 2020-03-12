## Cpu, Memory and Disk Simulation

Simulate CPU, memory and disk operations. <br>
- Physical and virtual have been implemented.
- Paging and page replacement with Second Chance FIFO algorithm.
- Page fault control.
- it is assumed that all instructions take 1 CPU cycle
-
- txt files contain the memory representations of program instructions and data. program instructions codes are at the end of the files.
- sort.txt can take much more time than search.txt
- memory changings and page replacement are displayed

### Usage

```sh
make
./Simulate <file_name> <debug_mode>
```

### Debug Modes <br>
- 0: run until end of the program and display memory content
- 1: run until end and print memory after all CPU cycles.
- 2: run 1 CPU cycle, print memory and wait for input to next step
- 3: run without printing memory

