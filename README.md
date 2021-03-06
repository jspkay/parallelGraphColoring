# ParallelGraphColoring
###### An implementation in C++

---

This educational project aims to compare the performances of different multithreaded graph coloring algorithms, having as a reference point a sequential greedy algorithm, which, although trivial, can have its benefits on small graphs.

## Contents
1. [Build instructions](#building)
   1. [Linux](#linux_build)
   2. [Windows](#windows_build)
2. [Files and classes](#files_and_classes)
   1. [main.cpp](#main_cpp)
   2. [Graph.h](#graph_h)
   3. [Algorithms.cpp](#algorithms_cpp)
   4. [ReadInput.cpp](#readinput_cpp)
3. [Usage](#usage)
   1. [Interactive Mode](#interactive_mode)
   2. [Command-line Mode](#cmd_mode)
   3. [List-files option](#list_files_option)
   4. [Command line parameters](#cmd_setup)
4. [Benchmark](#benchmark)
5. [Results](#results)

---

## Build instructions <a name="building"></a>
The project is developed using cmake. Platform specific instructions are available.
### Linux <a name="linux_build"></a>
It is necessary to install the following:
- cmake (>=3.17),
- make,
- boost program-options library,
- boost filesystem library.

On ubuntu it's possibile to execute the following command:
```
# apt install cmake libboost-program-options-dev libboost-filesystem-dev
```

Then, in the directory of the project:
```
$ cmake . && make
```
The binary will be copied in the root directory as well, ready to execute.

### Windows <a name="windows_build">
The program has not been tested on Windows, but the compilation process is similar to what's done on Linux.

First, downlaod the boost library binaries from the 
[official server](https://sourceforge.net/projects/boost/files/boost-binaries/) and install them. 
Thanks to cmake, the binaries will automatically be located and the compilation is performed exactly as in linux:
```cmd
cmake .
make
```
---

## Files and classes <a name="files_and_classes"></a>
The following files-structure represents the core files of the project.

```bash
.
|-- benchmark.sh
|-- CMakeLists.txt
|-- Graph
|   |-- Algorithms.cpp
|   |-- GraphAdjL.cpp
|   |-- GraphAdjM.cpp
|   |-- GraphCSR.cpp
|   |-- Graph.h
|   `-- readInput
|       |-- ReadInput.cpp
|       `-- ReadInput.h
|-- main.cpp
|-- PrintMenu.h
|-- README.md
`-- runlim.sh
```

The core files are explained in the following sections.

The only file which is essential for the building but not strictly related to the project is `CMakeLists.txt` which contains instructions about compilation. For further information, check [cmake system](https://en.wikipedia.org/wiki/CMake#:~:text=In%20software%20development%2C%20CMake%20is,generates%20another%20system's%20build%20files.)

### main.cpp <a name="main_cpp"></a>
The `main.cpp` file hold the interface for the software. Here the command line is parsed and the graph is constructed. In this section of the program, the main parameter of the colorization are chosen based on the user input. Finally, the algorithm is called n_trials times. 

### Graph/Graph.h <a name="graph_h"></a>
This file is where the magic happens. It contains the definition of a namespace ("asa" stands for Andrea, Salvo, Antonio, the authors) which was originally created to remove ambiguity between names. In later versions the ambiguity disappeared but the namespace stayed. 

The `Graph class` is defined inside the namespace . This class implements the CRT Pattern, in order to in order to support the use of different graph's representations. Originally the idea was to use an abstract class, but then we decided then to use this solution due to better performances and readibility. 
> Sidenote: the original version is still available in the branch "generalized_version"

The actual parallel algorithms are implemented inside the `Graph class`. Thanks to CRTP, the derived classes inherit the methods which have to be written just once, regardless of the internal representation of the graph. With this purpose in mind, some functions have to be specialized. Clearly, those functions are strictly related to different methods used for each representation and they are: `forEachNeighbor`, `forEachVertex` and `getDegree`. 

The specializations of the class are declared at the end of the same file. The implementations are to be found in the related `.cpp` files, which will not be explained in details, since they are self-explanatory.

### Algorithms.cpp <a name="algorithms_cpp"></a>
This file contains the actual implementations of each algorithm. Each algorithm has its own template function. 

One important aspect to underline is the presence of lines such as `template class asa::Graph<asa::GraphCSR>` in the end of the file. These are needed to the compiler so that it knows which version of the template should be compiled, since the template functions reside in a `.cpp` file.

### ReadInput/ReadInput.cpp <a name="readinput_cpp"></a>
This file contains the `ReadInput` class and its header file is `ReadInput.h`. It allows to generalize the step of acquisition data and consists into the set of variables and methods useful for reading two different kinds of graph input file (.graph and .gra), in order to store their edges into a vector of edges managed as a pair of nodes `std::vector<std::pair<node, node>> edges`. Then, these edges will be passed to the Graph class which construct the actual graph inside the program.



---

## Usage <a name="usage"></a>
The program can be run both in interactive mode and in command line mode, ie by entering the necessary parameters from the command line. 

### Interactive mode <a name="interactive_mode"></a>
To start the program in interactive mode, i.e. without selecting the desired options from the command line, but through a stdin and stdout, you must select the *help* option `GraphColoring --help` and then digit 'yes'.
The program will prompt a simple interactive interface to choose the input graph, the algorithm and so on.
Note that in this case, the input folder where you can select the input file will be "./Graph/benchmark", **fixed**.
<br>The following commands are available:

* **fin**, allows to list avaible input files and select the choosen one among them (Graph/benchmark directory).
  * by default = rgg_15 graph
* **ir**, allows to select the internal representation of the graph.
  * by default = CSR
* **a**, to select the algorithm to run.
  * by default = sequential
* **ct**, in order to select the number of thread to be run(*). Note that the meaning of this parameter heavily depend on the chosen algorithm.
  * by default = 1
* **nt**, to select the number of trials to be executed. At the end of the program, the average time(**) spent is displayed.
  * by default = 2
* **start**, to start the coloring (***).
* **q**, to quit the program.
<br><br>
*(\*) In the case of the sequential algorithm, the selected thread number will be irrelevant and in any case equal to 1.*<br>
*(\*\*) The average time is calculated as the sum of the time spent by the program in each nth run divided by n.*<br>
``` float(end_alg_time - begin_alg_time) / CLOCKS_PER_SEC;  ``` <br>
*(\*\*\*) You should choose this option only at the end, when you have already selected the necessary parameters.*<br>

### Command-line mode <a name="cmd_mode"></a>
It is also possible to run the program through command line options. To consult the program details directly from the terminal, you can print an infos window using the -h command (or --help).
The available options are the same as explained above, to which the command -l (--list-files) must be added.

### List-files option <a name="list_files_option"></a>
Directory organization:
```
.
`-- The
    `-- Powerpuff
        `-- Girls
            |-- Molly
            |-- Dolly
            `-- Lolly

```
It's possibile to use the option -l (--list-files) to show a complete list of the files in the chosen path  numbered in alphabetical order. This is needed because the file is chosen through a number with the option "input" (-i)

```
$ GraphColoring -p The/Powerpuff/Girls --list
0 - Dolly
1 - Lolly
2 - Molly
```
So it's possible to run the program on a specific file:
```
$ GraphColoring -p The/Powerpuff/Girls -i 1 
```
In this way the program will run with the default(*) settings.
For the other settings, please refer to the next sections.

> In this repository there are some benchmark files in `Graph/benchmarks`

*(\*) For the default values of existing options, see interactive mode.*

### Command line arguments <a name="cmd_setup"></a>
Command line parameters are handled as "positional arguments", therefore it is necessary to write them down in a fixed order:
`input, internal_representation, algorithm, threads, trials`

> NOTE: Even though "positional arguments" may be used, also the standard GNU style are anyway allowed

It should be noted that in principle the corresponding options should not be written explicitly as they are "positional", but still possible:
``` -a 3 instead of 3 ```, and it may be useful to use them in this explicit way if we want to start the program with only some of the default parameters changed.
<br> Lastly, it's possible to change the path of the input directory with the option -p. 

As examples of usage:
```bash
GraphColoring 0 1 2 2;                    #predefined path, default number of trials (=1)
GraphColoring -i 2 -n 5;                  #predefined path, default internal representation (=CSR), default algorithm (=sequential)
GraphColoring -p ../input-dir -i 2 -t 5;  #default internal representation, algorithm
GraphColoring -l;                         #list input files in the predefined input directory
GraphColoring -p ../input-directory -l;   #list input files in the chosen input directory
```

---

# Benchmark <a name="benchmark"></a>
To test program performanes as best as possible, we used a tool called 'runlim' which samples resources usage during the run of the program. Through the output log of the tool it's possible to know two significant aspects: 
   * the **space** required by the program to run
   * **time** necessary to run the coloring algorithm selected
      * cfr *wall time* and *cpu time* on report document  
   
In order to run multiple times runlim on `GraphColoring` and compute consequently the average time and space spent by this, `runlim.sh` has been written.
It takes as arguments:
   1. `PROGRAM_STRING` - the enquoted string which need to be passed to the `runlim executable
   2. `nTrials` - the number of trials to be executed

As examples of usage:
```bash
runlim.sh 'GraphColoring 0 1 2 2' 5;   #5 trials                   
runlim.sh 'GraphColoring -i 2 -a 4' 10;  #10 trials                
```

The string parameter is the exact program which runlim will execute. In this example, runlim will execute `GraphColoring -i 2 -a 4` as it were executed in the shell on its own. 

To test a single configuration of `GraphColoring` parameters, this script would be enough. As we are lazy and not very eager to start a thousand tests with variable parameters, `benchmark.sh` was created.
   
The purpose of `benchmark.sh` bash script is to iterate on different algorithms, input files, value of threads number used and mixing them: so execute all possible **configurations**. 

The script accepts two different parameters:
1. `nThreads` - the maximum number of logical threads the program has to create. The program will be executed log2(nThreads) times, because it will be run with an increasing number of threads in powers of 2 starting exaclty from 2.
2. `nTrials` - the number of trials to be executed per configuration. Note that this parameter will be passed to `runlim.sh`
   
Examples:
```bash
benchmark.sh 4; #max 4 threads and only one trial per configuration
benchmark.sh 8 3; #max 8 threads and 3 trials per configurations with average time computation.
```

The `benchmark.sh` script report logs the results in one file, whose name can be edited as explained below

Last thing to be said is that some parameters are of the `benchmark.sh` script can be modified editing the script itself. In the first part of the file a section `Editable parameters` can be found. At the present it's possibile to edit the following:
- `file_list` - this is an array containing the indices of the used file as specified by `./GraphColoring -p /path/to/benchmarks -l`
- `algorithms` - similarly, it's the list of the indices of the algorithms to benchmark. The correspondance between indices and algorithms it's shown at the top of the script `benchmark.sh`
- `log_file` - is the name of the file in which the results will be stored.

Results obtained can be read in the report.

# Results <a name="update"></a>
For a comprehensive overview of the results, please consult the document (`Parallel_graph_coloring.pdf`) in this repository.

What we noticed is an impairment between performances and expectations. The resulsts show that our parallel algorithms are much slower than the greedy sequential one. This is most likley due to the OS resources management, which tries to optimize the distribuion instead of giving every thread to the same process.
