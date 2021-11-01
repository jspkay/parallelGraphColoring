






## Algorithms explanation
### Sequential
This algorithm is the simple greedy sequential (run with only one thread).

### Jones-Plassmann and LDF
Jones-Plassmann's algorithm is very similar to the Largest-Degree first algorithm. In this implementation a number of threads n+1 run in parallel.

> n is the number of thread specified in the options

This implementation is designed to make the n threads operate on disjoint data, so that the use of locks can be limited (making the algorithm run faster). The algorithm runs in two phases:
- Fase 1. The n threads run in parallel, calculating first their range of operation (i.e. the range of vertices to check). Then, each thread check the assigned vertices one by one to select the MIS depending on the random value. Here a lock is needed to update the latter set. The main thread here just wait for the n threads to complete their job.
- Fase 2. The n threads stop and wait for the main to update the condition to make the n threads start again. At this moment the n threads run in parallel with the main thread. The main thread just update the number of threads colored (used for the termination condition) while the n threads color the vertices (always in a disjoint fashion). At the end of this process all the main thread wait for the others to complete and the cycle repeat.

#### LDF
The LF algorithm works in the same way as the JP with the difference that the MIS is chosen in based on the degree of the nodes. First the maximum degree node (in the neighborhood) and if two nodes have the same degree a random value is looked at.

### LDF - modified version
Since the degree is fixed it is possible to run n parallel threads which cycle together through the vertices and check which one has the maximum degree in the neighborhood and (if not colored already) colors it. This observation is the main idea behind this implementatin.

### SMF


## Files organization
- main.hpp. It has the main function and the basic structure of the program, like the option parser, case switches and time counter.
- PrintMenu.hpp. It contains the interactive menu functions and some other information useful to interact with the running program
- Graph/
    - Graph.h contains the general definition of a graph class, comprised of the algorithms, variables and methods which are common to the derived classes. The derived classes are implemented with the CRTP, in order to get the best performances.
    - GraphAdjL.cpp, GraphAdjM.cpp, GraphCSR.cpp contain the specialization of the Graph class.
    - Algorithms.cpp contains the actual algorithms' code, where the low level implementation can be examined. 


