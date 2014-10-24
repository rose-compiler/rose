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

          GridFunction operator()(GridFunction & u);

          Operator operator+(const Operator & x);
          GridFunction operator=(const D & d);

   };




