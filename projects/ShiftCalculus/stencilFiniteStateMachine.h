#ifndef STENCIL_FINITE_STATE_MACHINE_H
#define STENCIL_FINITE_STATE_MACHINE_H

// This defines the design of the finite state machine for the stencil abstraction.
// The finite machine is required because we cannot execute the code that we see 
// at compile time, and so must emulate it.  We can do this because the operations
// defined in the stencil specification have clear semantics and the stencil specification
// uses a restricted subset of C++ which does not require runtime evaluation.  
// This is a critical point in the design and use of the stencil specification and one
// leveraged in the interpretation of it at compile time.  More analysis at compile-time
// might permit additional flexability in stencil specificaion in the future (if it
// is required).

// There are specific events defined for this finte state machine:
//    1) the creation of a stencil 
//    2) the operation to add a shift/coeficient pair to an existing stencil
//    3) operatrs on stenciles
//       a. union
//       b. convolution


class StencilOffsetFSM
   {
  // This abstracts the concept of an offset and is similar to the "Point" class in the Stencil DSL.
  // The difference is that this need not be as flexible in the dimension as long as we can hold
  // the largest dimentions stencil that the Stencil DSL can define (I think this value is 3 dimensions).

     public:
          int offsetValues[3];

       // The default is to build a zero offset.
          StencilOffsetFSM(int x = 0, int y = 0, int z = 0);

          StencilOffsetFSM operator*=(int x);

          void display(const std::string & label);
   };

class StencilFSM
   {
  // This finte state machine operates via speicific events and
  // records the state transitions so that the result is a data
  // struction defining the stencil and which we can interogate 
  // to generate code for the stencil at compile time.

     public:
          std::vector<std::pair<StencilOffsetFSM,double> > stencilPointList;

     public:
       // Default constructor.
          StencilFSM();

       // This is the simplest stencil (a single point).
          StencilFSM(StencilOffsetFSM x, double stencilCoeficient);

       // Union operator
          StencilFSM operator+(const StencilFSM & X);

       // Convolution operator
          StencilFSM operator*(const StencilFSM & X);

       // Assignment operator (operator=).
          StencilFSM operator=(const StencilFSM & X);

       // Copy constructor.
          StencilFSM (const StencilFSM & X);

          void display(const std::string & label);

       // Access functions: Reports the width of the stencil (edge to edge).
          int stencilWidth();

       // Access functions: Reports the dimensionality of the stencil.
          int stencilDimension();
   };


// endif for STENCIL_FINITE_STATE_MACHINE_H
#endif
