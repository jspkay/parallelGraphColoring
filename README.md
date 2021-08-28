# Parallel Graph Coloring
###### an implementation in C++

## Structure of the project
This project has three main classes:
- Input,
- Graph,
- graphColoring.

The basic idea is to provide easy-to-swap components in order to best test each algorithm independently of the actual representation of the graph. All the classes are encapsulated in a namespace which is `asa` (as the name of the authors). Now each class will be described in order to better understand its purpose. 

> Note: this project uses concept. For this reason is it mandatory to either use a C++20 compiler (e.g. clang10, c++-10) or remove the concept from the `Input` class.

### 1·Graph
This is the backbone of the project. This is a pure virtual class whose purpose is to provide access to the graph without knowing the internal representation. It could be described as an interface.

In order to use this class it is necessary to extend it, providing the actual methods that manipulate the graph.

### 2·Input 
This only serves the purpose of reading a file. It is a template class because it constructs the graph object and in order to do that the specific class has to be used.

> NOTE: The name of this class has to be decided yet, since it will also provide output (and maybe statistics)

### 3·graphColoring
This class contains all the algorithms which actually color the graph, both in parallel or sequentially. This is where the magic happens, because the algorithms can be programmed though the Graph interface.

> NOTE: This class is also a good candidate for output and statistics.

## Usage
The usage is quite simple. For each task a class is needed.

The first thing one wants to use is the class `Input`. It has a static method which gets a filename and returns an object whose class is specified through the template. 

In the following example a graph of type `Graph_simple` is created:

` Graph_simple myGraph = Input<Graph_simple>::readInput(filename);`

The second step is to import the graph inside a `graphColoring` object:

`graphColoring gc (&myGraph)`

> **IMPORTANT**: notice that graphColoring constructor has a pointer to a Graph object as parameter! This means that the Graph object **has to exist** outside the graphColoring object.

The last step is to set randoms (if necessary) and call the appropriate method of the class graphColoring:

```
gc.setRandoms(1000);
gc.sequentialAlgorithm();
```

## Extending the behavior
The library is made keeping in mind everything should be simply swappable. That's why it's possible to extend the classes.
I will describe what is important to know in order to do that for each class.

### Input
To create a new class in order to read another type of file, it's possible to just create it as a template making sure to use the concept `isGraph`:
```c++
template <asa::isGraph T>
class MyInput{
    static T readInput(const std::string& filename);
}
```
The method readInput has to prepare the graph using three important methods:
1. First goes the constructor. It accepts two paramenters which represent respectively the number of verteces and the number of edges of the graph.
2. Then every edge is added through the `addEdge` method.
3. It is mandatory to call the method `finalize` for reasons which are explained in the next section.

### Graph
To change the internal representation of the graph, maybe to use a more efficient one, it is recommended to extend the Graph class and implement the methods as in the example:
```c++
    class MyGraphClass : public Graph {
        // variables needed for the graph
        adjacencyListGraph g;

    public:
        // construction methods
        MyGraphClass(unsigned long V, unsigned long E);
        void addEdge(node a, node b) final;
        void prepare() final;

        // manipulation methods
        void forEachVertex(std::function<void(unsigned long)> f) final;
        void forEachNeighbor(unsigned long v, std::function<void(unsigned long)> f) final;
        unsigned long getDegree(unsigned long V) final;
    };
```
Each method should be marked as final (or at least override), to increase performances.
The `prepare` method is used to finalize the graph. For example, when using Boost CSR graph, the actual construction of the object happens after a vector has been filled with all the edges.

The other methods have to be implemented as the name describe.

### graphColoring
The extension has to be prepared yet, but it's possible to create a class which use the Graph interface methods.

## Available graphs and algorithms
A couple of algorithms and graph representations are available to use already. The full list whit description follows.
### Graphs
- Graph_simple - This implements the classic adjacency list representation. It's made using a `vector` of `forward_list`s of `int`.
- Graph_csr - Implements the boost compressed sparse row representation.

### Algorithms
They're all implemented in the `graphColoring` class.
- sequentialAlgorithm - Simple [greedy algorithm](https://en.wikipedia.org/wiki/Greedy_coloring)
- largestDegree - Sequential version of the largest degree first algorithm.