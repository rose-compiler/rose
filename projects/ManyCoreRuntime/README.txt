This project represents a runtime system to support many core optimizations.

A specific focus is on exascale architectues and supporting manycore
processors that will have either no cross-chip cache coherency or 
poor performance of any possible cross-chip cache coherency.

As a model exascale machines we assume:
   1) A million processors (this runtime system is mostly independent of distributed
      memory processing).
   2) A thousand cores per processor (to be thought of as any combination of CPU cores or
      GPU cores).
   3) Zero cross-chip cache coherency.


This work is a draft of an evlving many-core runtime system that will fit with 
other programming model building blocks.


A goal if for this work to be coupled with a compiler to support many-core
optimizations for HPC applications.  Stencil operations are a specific target.



