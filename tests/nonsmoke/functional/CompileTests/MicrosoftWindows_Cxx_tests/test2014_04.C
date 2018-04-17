// This test code demonstrates that the static const member data for a 
// primative (and non-floating point) must be initialized in the class
// because it is used in the class (I guess).
// two places, but not both.

const int a = 42;
int b = 7;

class A
   {
     public:
       // This will unparse without the initializer for the constant (which is allowed in the class for integers (and primative non-floating point types)..
          static const int n = 12;
       // static const int n;

          int array[n];
   };

// const int A::n = 12; // error
// const int A::n;

// This is allowed, if the const value is initialized either in the class or in the variable definition.
// int another_array[A::n];
