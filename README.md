# ParallelGraphColoring
###### An implementation in C++
This educational project aims to compare the performances of different multithreaded graph coloring algorithms, having as a reference point a sequential greedy algorithm, which, although trivial, can have its benefits on small graphs.
## How to build
CMakeLists.txt file, in the root directory, contains a set of directives and instructions describing the project's source files and targets (executable, library, or both). For the sources of this project, refer to that file.
Directly linked to the latter, there is the cmake_install.cmake file in the /cmake-build-debug folder, where you can type the command "cmake" to build the program executable with all the necessary components.

## Usage
The program can be run both in interactive mode and in "stand-alone" mode, ie by entering the necessary parameters from the command line. 

## Interactive mode
To start the program in interactive mode, i.e. without selecting the desired options from the command line, but through a stdin and stdout, you must select the help option and then press yes.
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

## Stand-alone mode
It is also possible to run the program through command line options. To consult the program details directly from the terminal, you can print an infos window using the -h command (or --help).
The available options are the same as explained above, to which the command -l (--list-files) must be added.

### List-files option
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
$ GraphColoring The/Powerpuff/Girls --list
0 - Dolly
1 - Lolly
2 - Molly
```
So it's possible to run the program on a specific file:
```
$ GraphColoring The/Powerpuff/Girls -i 1 
```
In this way the program will run with the default(*) settings.
For the other settings, please refer to the next sections.

*(\*) For the default values of existing options, see interactive mode.*

###Command line setup
Command line parameters are handled as "positional arguments", therefore it is necessary to write them down in a fixed order:
``` input, internal_representation, algorithm, threads[, trials] ```.<br>
It should be noted that in principle the corresponding options should not be written explicitly as they are "positional", but still possible:
``` -a 3 instead of 3 ```, and it may be useful to use them in this explicit way if we want to start the program with only some of the default parameters changed.

As examples of usage:
```bash
GraphColoring 0 1 2 2;                  #path predefined, number of trials remains 1 by default
GraphColoring 1 1 1 2 5;                #path predefined
GraphColoring -i 2 -t 5;                #path predefined, internal representation, algorithm, trials by default
GraphColoring ../input-dir -i 2 -t 5;   #internal representation, algorithm, trials by default
GraphColoring -l;                       #to list input files in the predefined input directory
GraphColoring ../input-directory -l;    #to list input files in the chosen input directory
```

#Benchmark