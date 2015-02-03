// This header file attempts to define classes that would permit 
// a C++ API that would drive the generation of the the Maple code
// and then generate the more efficent code to be used within a application.

// We expect the generated code to be large.
class D
   {
  // This is a common base class for D_plus and D_minus below.
   };

class D_plus : public D
   {
     public:
          D_plus(int dim);

       // I think this is not used.
       // D_plus operator+(const D & d);

          D_plus operator()(const D & d);
   };

class D_minus: public D
   {
     public:
          D_minus(int dim);

       // I think this is not used.
       // D_minus operator+(D d);

          D_minus operator()(D d);
   };

   
class GridFunction
   {
     public:
       // Allow simple assignment in API.
          GridFunction & operator=(double x);

       // Allow assignment of GridFunction objects (such as when the evaluated operator is the rhs).
          GridFunction & operator=(const GridFunction & x);

       // Allow output function in API.
          void print();

       // This is need to support code generation.
          double* getPointer();
   };

class Operator : public D
   {
  // Operator have dimension from the pieces that define them.

     public:
          Operator();
          Operator(D d);

       // Note that "Operator Lap2D = DPDMx + DPDMy;" evaluates more explicitly to
       // "Operator::operator=(Operator(DPDMx) + Operator(DPDMy));" where the 
       // operator=() function is default generated.
          Operator operator+(const Operator & x);

       // This is the application of the Operator to the GridFunction.
          GridFunction operator()(GridFunction & u);

       // This function is not need in the API.
       // GridFunction operator=(const D & d);
   };



