CBop
====
[![Build Status](https://travis-ci.org/bop-langs/cbop.svg)](https://travis-ci.org/bop-langs/cbop)


CBop is a library similar to OpenMP but important safety features added on. CBop allows developers to quickly and safely add parallelization to complicated projects easily. Unlike OpenMP, CBop runs parallel regions *speculatively* and if an error in sequential consistency is found aborts the parallel attempt and resorts to a sequential execution automatically.

To denote a parallel region, call ```BOP_ppr_begin(int)``` with a region id. All code between this call and a call to ```BOP_ppr_end(int)``` will be executed n parallel. For correctness and the safety guarantees, you must mark accesses to global memory with their corresponding functions. For examples, see the tests/ directory.


##Compile time flags

To use the cbop library, you need to compile with ```-fPIC``` (force position independent code) and ```-D BOP``` (define BOP macro as 1). Without fPIC flag, overloading malloc will not work and cbop will have undefined behavior if it compiles. At link-lime time, -ldl needs to be specified.
