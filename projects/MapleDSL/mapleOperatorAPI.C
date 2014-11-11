// This is the implementation of the Maple Operator used to 
// pass information about the maple generated data to ROSE
// so that we can support the code generation.

#include "rose.h"

// This code will make calls to the finite state machine representing the stencil 
// so that we can execute events and accumulate state (and then read the state as
// and intermediate form for the stencil (maybe saved as an attribute).  This data
// is then the jumping off point for different groups to experiment with the generation
// of architecture specific code.
// #include "operatorFiniteStateMachine.h"

// #include "operatorEvaluation.h"

// #include "dslSupport.h"

// Added because we call this from the SgExprStatement in the synthisized attribute evaluate.
// #include "dslCodeGeneration.h"

#include "mapleOperatorAPI.h"

using namespace std;


StencilPoint::StencilPoint()
   {
  // default constructor.

     isNonzero        = false;
     coefficientValue = 0.0;
   }

StencilPoint::StencilPoint(bool temp_isNonzero, double temp_coefficientValue)
   {
     isNonzero        = temp_isNonzero;
     coefficientValue = temp_coefficientValue;
   }

StencilPoint::~StencilPoint()
   {
#if 0
     printf ("StencilPoint::~StencilPoint() \n");
#endif
   }



StencilOperator::StencilOperator(int size_x, int size_y, int size_z)
   {
     isOperatorWieghtedStencil = false;

  // This demonstrates a simple construction of a multi-dimensional array using 1D STL vectors.
  // It is not that I am a big fan of this approach, but I needs a multi-dimensional array 
  // abstraction.  In general there would be a perfomance penalty for this, but in our usage
  // these arrays will be less than 10x10x10 at the largest.

     for (int k = 0; k < size_z; k++)
        {
          vector < vector < StencilPoint > > plane;
          for (int j = 0; j < size_y; j++)
             {
               vector < StencilPoint > row;
               for (int i = 0; i < size_x; i++)
                  {
                    StencilPoint p;
                    row.push_back(p);
                  }
               plane.push_back(row);
             }
          stencil.push_back(plane);
        }

  // This demonstrates a simple access of the stencil's dense multi-dimensional data strcuture.
  // It at least provides a simple access that is intuative. Performance is not a problem because
  // out usage is always going to be very small.
     for (int k = 0; k < size_z; k++)
        {
          for (int j = 0; j < size_y; j++)
             {
               for (int i = 0; i < size_x; i++)
                  {
                    stencil[k][j][i].isNonzero = false;
                    stencil[k][j][i].coefficientValue = 0.0;
                  }
             }
        }
#if 0
     printf ("StencilOperator constructor: size_x = %d size_y = %d size_z = %d \n",size_x,size_y,size_z);
#endif
   }

StencilPoint & StencilOperator::operator()(int i, int j, int k)
   {
#if 0
     printf ("StencilOperator::operator() i = %d j = %d k = %d \n",i,j,k);
#endif
     return stencil[k][j][i];
   }

StencilOperator::~StencilOperator()
   {
#if 0
     printf ("StencilOperator::~StencilOperator() \n");
#endif
   }




