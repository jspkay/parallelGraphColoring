# Classes and inheritance

In order to test effectively different kinds of internal representations and algorithms, without the effort of write the algorithm for each representation, I introduced two kinds of classes:

- Graph, which are just an abstraction (or an interface, if you will) of the internal representation of the graph
- graphColoring, which implements the actual algorithms.

I had to make a choice with respect to the type of class I wanted to use for the Graph class. That's due to the fact that each time a new representation is needed, a new class ("implementing" Graph) is also needed. I tried solving the problem using the CRTP but I failed when I noticed that graphColoring has to have some sort of reference to the actual graph. The obvious choice, then, was to use an abstract class with virtual methods. This helped a lot in the development of representation-independent algorithms. 
Also, each specialized virtual method is marked final, so that there is no need to use the V-Table, [as explained here](
https://devblogs.microsoft.com/cppblog/the-performance-benefits-of-final-classes/).
The actual drawback of this strategy is the need to use a pointer to get access to the graph, but that's negligible, considering that it doesn't alter the performances.