# CWUPCBSchedulingSimulator
Milo McCarty
11/30/2022

This is a lab for simulating a CPU executing processes on different cores with different algorithms.
The cases for each algorithm is:
1 First Come First Serve
2 Round Robin
3 Shortest Job First
4 Priority Scheduling

To run the code:
`g++ CWUPCBSchedulingSimulator.cpp -pthread`
`./a.out PCB.bin <algo 1> <percentage 1> <algo 2> <percentage 2> ... <algo n> <percentage n>`

Replace <algo> with a case 1-4 and <percentage> with a value between 0 and 1. All percentages should add up to 1.0
When one core runs out of processes, the remaining processes will be evenly split between all cores.