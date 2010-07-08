// Test of unparsing const variable declarations
// Although it seems strange KCC accepts
//      class A const X;
// as valid C++ code.

/*
// original code:
const B X;
// unparsed code:
class B const X; 
 */


class A
   {
     public:
          A(){};
   };

// This is unparsed as: class A const X; (which seems to be valid C++ code)
const A X;
