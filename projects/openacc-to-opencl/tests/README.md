
# Tests for the OpenACC to OpenCL compiler

Common files:
 * make-environment.mk
 * Makefile
 * README.md

## Tests level 0

Implementation tests.

List:
 * 001-iteration : Tests the iteration domain and the distribution accross gang, worker, vectors and the tiles. (TODO: improve + automated test)

## Tests level 1

One kernel in one region.

List:
 * 101-vector-constant-addition     : Adds constant value to each elements of an array, ~ **n** ops
 * 102-vector-constant-product      : Multiplies each elements of an array by a constant, ~ **n** ops
 * 103-vector-vector-addition       : Computes the sum of two vectors, ~ **n** ops
 * 104-saxpi                        : TODO y[i] = alpha*x[i] + y[i], ~ **n** ops
 * 105-vector-vector-multiplication : TODO Computes the product of the elements of two vectors, ~ **n** ops
 * 106-matrix-matrix-addition       : TODO Computes the sum of two matrices, ~ **n.m** ops
 * 105-vector-vector-product        : TODO Computes the scalar product of two vectors, ~ **n + log(n)** ops
 * 106-matrix-vector-product        : TODO Computes the product of a matrix _n,m_ by a vector _m_, ~ **n.m + n.log(m)** ops
 * 107-vector-matrix-product        : TODO Computes the product of a vector _n_ by a matrix _n,m_, ~ **m.n + m.log(n)** ops
 * 109-matrix-matrix-product        : TODO Computes the product of two matrices _n,p_ and _p,m_, ~ **n.m.p + n.m.log(p)** ops

Content:
 * Makefile
 * 1xx-init.c       : Functions to initialize and launch the kernel
 * 1xx-openacc.c    : OpenACC version of the kernel
 * 1xx-klt.lt       : Description of the kernel in KLT language
 * 1xx-libopenacc.c : **Hand-written** Transformated OpenACC kernel
 * 1xx-host-data.c  : **Hand-written** Generated regions and kernels descriptor
 * 1xx-kernels.cl   : **Hand-written** Generated OpenCL C kernels
 * 1xx-test-vars.rc : Configuration for the 

Related files:
 * 100-main.c       : Generic _main_
 * 100-rules.mk     : Generic make rules
 * 100-eval.sh      : Evaluation script
 * 100-process.sh   : Proccessing script
 * 100-plot.sh      : Plotting script
 * 100-test-vars.rc : Variables values for evaluation

## Tests level 2

Multiple kernels in one region.

List:
 * 201-matrix-matrix-vector-product : TODO Computes the product of two matrices _n,p_ and _p,m_ and a vector _m_, ~ **n.m.p + n.m.log(p) + n.m + n.log(m)** ops


