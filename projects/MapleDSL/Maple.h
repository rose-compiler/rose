// This header file attempts to define classes that would permit 
// a C++ API that would drive the generation of the the Maple code
// and then generate the more efficent code to be used within a application.

class GridFunction
   {
     public:
       // We could also define the symetric version of the operator.
          friend GridFunction operator*(double a, const GridFunction & u);
          friend GridFunction operator*(const GridFunction & u1, const GridFunction & u2);
   };

// We expect the generated code to be large.
class D
   {
   };

class CoefficientFunction
   {
  // This might be used as the return type of algebraic equations on operator D.
     public:
          CoefficientFunction();
          CoefficientFunction(const GridFunction & u);

       // We could also define the symetric version of the operator.
          GridFunction operator*(const GridFunction & u);

          GridFunction operator()();
   };


// Forward declaration
class Operator;

class D_plus : public D
   {
     public:
          D_plus(int dim);

       // This operator is used for "dpx(dmx(a*u)) + dpy(dmy(b*u))".
          friend Operator operator+(const D_plus & dx, const D_plus & dy);

       // D_plus(double constant_coefficient_value);
       // This permits use of arbitrary coefficients (even functions)
          friend D_plus operator*(double a, const D & d);

          D_plus operator()(const D & d);
          D_plus operator()(const GridFunction & u);
   };

class D_minus: public D
   {
     public:
          D_minus(int dim);

       // This permits use of arbitrary coefficients (even functions)
          friend D_plus operator*(double a, const D & d);

          D_minus operator()(D d);
          D_minus operator()(const GridFunction & u);
   };

   
class Operator : public D
   {
  // Operator have dimension from the pieces that define them.

     public:
          Operator();
          Operator(D d);

          Operator(D_plus d);

       // This is the application of the Operator to the GridFunction.
          GridFunction operator()(GridFunction & u);

       // Note that "Operator Lap2D = DPDMx + DPDMy;" evaluates more explicitly to
       // "Operator::operator=(Operator(DPDMx) + Operator(DPDMy));" where the 
       // operator=() function is default generated.
       // Operator operator+(const Operator & x);
          friend Operator operator+(const Operator & x, const Operator & y);
          friend Operator operator+(const D_plus & dx, const D_plus & dy);
   };




