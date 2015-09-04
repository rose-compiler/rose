// This is the simple version of laplacian.cpp that builds 
// the smallest reasonable AST.  This simpler AST make it 
// easier to work with in the context of understanding 
// the use of the DSL and the interpretation of it's semantics 
// to implement the code generation for the DSL.

// This is the second simple test that applies a Laplacian operator
// with 2*DIM+1 size stencil with one ghost cell along each face
// of the data box. Thus the size of the destination box is 
//BLOCKSIZE^DIM and the size of the source box is (BLOCKSIZE+2)^DIM
//FOR these tests BLOCKSIZE=DOMAINSIZE because we are working with 
//a single box. BLOCKSIZE, DOMAINSIZE and DIM are set in the Makefile


#define BLOCKSIZE 32
#define DIM       3

#include <vector>
// #include <list>

#include <memory>
// #include <array>

#include "laplacian_lite_test_variableMapping.h"

int main(int argc, char* argv[])
   {
     Stencil<double> laplace;//  = C0 * (S^zero);

   }
