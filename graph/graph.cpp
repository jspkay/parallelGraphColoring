//
// Created by salvo on 11/08/21.
//

#include <iostream>
#include <openssl/ossl_typ.h>
#include "graph.h"

graph::graph() {
    // verteces call default constructor
    n = 0;
}

graph::graph(unsigned long int n) {
    verteces = std::vector<std::forward_list<int>>{n};
    for(auto &el : verteces) el = std::forward_list<int>{};
    this->n = n;
}

void graph::addEdge(int a, int b) {
    if(a == b){
        std::cout << " No self loop!\nexiting ";
        exit(-2);
    }

    int m = a > b ? a : b + 1;
    if(m > n){
        std::cout << "The graph only contains " << n << " verteces.\n exiting.";
        exit(-1);
    }

    // Check if the edge exist already
    for(auto &el : verteces[a]){
        if(el == b) return;
    }

    verteces[a].emplace_front(b);
    verteces[b].emplace_front(a);
}

std::vector<int> graph::getNeighbors(int n){

    return std::vector<int>{verteces[n].begin(), verteces[n].end()};

    /*std::vector<int> res{};

    for(auto el : verteces[n])
        res.push_back(el);

    return res; */
}

/*
I need to implement the greedy coloring algorithm. This are my files:
- [main.cpp][1]
- [graph.h][2]
- [graph.cpp][3]

Output if executed:
```
malloc(): corrupted top size

Process finished with exit code 134 (interrupted by signal 6: SIGABRT)
```

It stops when I execute the line res.push_back(el);. But, in general, the same error happens if I try to construct a vector<int> in the function getNeighbors.

The call stack from gdb:
```
#0  0x00007fad3147bd22 in raise () from /usr/lib/libc.so.6
#1  0x00007fad31465862 in abort () from /usr/lib/libc.so.6
#2  0x00007fad314bdd28 in __libc_message () from /usr/lib/libc.so.6
#3  0x00007fad314c592a in malloc_printerr () from /usr/lib/libc.so.6
#4  0x00007fad314c9334 in _int_malloc () from /usr/lib/libc.so.6
#5  0x00007fad314ca397 in malloc () from /usr/lib/libc.so.6
#6  0x00007fad3181044d in operator new (sz=4) at /build/gcc/src/gcc/libstdc++-v3/libsupc++/new_op.cc:50
#7  0x000055f5d490a5be in __gnu_cxx::new_allocator<int>::allocate (this=0x7fff6ce21c00, __n=1) at /usr/include/c++/11.1.0/ext/new_allocator.h:121
#8  0x000055f5d490a064 in std::allocator_traits<std::allocator<int> >::allocate (__a=..., __n=1) at /usr/include/c++/11.1.0/bits/alloc_traits.h:460
#9  0x000055f5d4909868 in std::_Vector_base<int, std::allocator<int> >::_M_allocate (this=0x7fff6ce21c00, __n=1) at /usr/include/c++/11.1.0/bits/stl_vector.h:346
#10 0x000055f5d490bdc3 in std::vector<int, std::allocator<int> >::_M_range_initialize<std::_Fwd_list_iterator<int> > (this=0x7fff6ce21c00, __first=2, __last=non-dereferenceable iterator for std::forward_list) at /usr/include/c++/11.1.0/bits/stl_vector.h:1582
#11 0x000055f5d490b985 in std::vector<int, std::allocator<int> >::vector<std::_Fwd_list_iterator<int>, void> (this=0x7fff6ce21c00, __first=2, __last=non-dereferenceable iterator for std::forward_list, __a=...) at /usr/include/c++/11.1.0/bits/stl_vector.h:657
#12 0x000055f5d490b446 in graph::getNeighbors (this=0x7fff6ce21ca0, n=1) at path/graph.cpp:43
#13 0x000055f5d4908392 in greedyAlgorithm (G=...) at path/main.cpp:13
#14 0x000055f5d49086dc in main () at path/main.cpp:44
#15 0x00007fad31466b25 in __libc_start_main () from /usr/lib/libc.so.6
#16 0x000055f5d490818e in _start ()
```

I am using C++14.


[1]: https://pastebin.com/yjSFJfrH
[2]: https://pastebin.com/Yb0AgkUi
[3]: https://pastebin.com/cm1NcZPZ

 */