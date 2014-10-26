// This header file attempts to define classes that would permit 
// a C++ API that would drive the generation of the the Maple code
// and then generate the more efficent code to be used within a application.

// We expect the generated code to be large.
class D
   {
   };

class D_plus : public D
   {
     public:
          D_plus(int dim);
          D_plus operator+(const D & d);

      D_plus operator()(const D & d);
          
   };

class D_minus: public D
   {
     public:
          D_minus(int dim);
          D_minus operator+(D d);

          D_minus operator()(D d);
   };

   
class GridFunction
   {
   };

class Operator : public D
   {
  // Operator have dimension from the pieces that define them.

     public:
          Operator();
          Operator(D d);

       // This is the application of the Operator to the GridFunction.
          GridFunction operator()(GridFunction & u);

       // Note that "Operator Lap2D = DPDMx + DPDMy;" evaluates more explicitly to
       // "Operator::operator=(Operator(DPDMx) + Operator(DPDMy));" where the 
       // operator=() function is default generated.
          Operator operator+(const Operator & x);

       // This function is not need in the API.
       // GridFunction operator=(const D & d);

   };




