# udgld (UnDirected Graph Loop Detection)

C++ wrapper over Boost Graph Library (aka as BGL), provides a mean to detect loops in undirected graphs.

For example, with the following graph, generated by include sample program:

![alt](
https://github.com/skramm/udgld/blob/master/obj/sample1_2.png "sample graph")

This code will give you the two loops, as two paths:
```
1-2-3-4-5-6
3-7-14-13-4
```

### Status
- Not extensively tested, but provides sample application code (that works).
- Modern C++ design'ed (RAII).
- Released under the Boost licence.

### Usage:
 - Include the file `udgld.hpp` in your application.
 - Call the \c UDGLD_INIT macro (outside of `main()` )
 - Build your graph (add vertices and edges, see BGL manual)
 - Call `udgld::FindLoops()`. It will return a set of paths that are loops.
 - Done !

See `sample_1.cpp` as example.


### Build:
- header only, no build.
- To build the provided sample code, just run makefile, no other dependency than BGL.
(tested with 1.54)

##### Build options:
 - if UDGLD_PRINT_STEPS is defined, then different steps will be printed on `std::cout` (useful only for debugging purposes). See makefile.

### Issues:
 - at present, this code requires a static member that you need to allocate memory for (see \c UDGLD_INIT).
 Thus it is not thread safe, neither can it handle multiple graphs simultaneously.

### References
 - For details on BGL, see http://www.boost.org/doc/libs/1_58_0/libs/graph/doc
 - [wikipedia.org Graph page](https://en.wikipedia.org/wiki/Graph_%28mathematics%29#Undirected_graph)
