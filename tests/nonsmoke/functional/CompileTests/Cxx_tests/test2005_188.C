// Demonstrate name qualification of parameter arguments

typedef enum enumType1
   {
     zeroA = 0,
     oneA,
     twoA
   };

class Array
   {
     public:

class ArrayB
   {
     public:

       // Error: This becomes a SgEnumDeclaration in the AST 
       // Is this a typedef which has no name?
#if 1
       // Within this scope ::enumType1 is hidden (should be in EDG's list of hidden names)
          typedef enum enumType1
             {
               zeroA = 0,
               oneA,
               twoA
             };
#endif
   };

       // Error: This becomes a SgEnumDeclaration in the AST 
       // Is this a typedef which has no name?
#if 1
          typedef enum enumType1
             {
               zeroA = 0,
               oneA,
               twoA
             };
#endif
          enum enumType4
             {
               zeroD = 0,
               oneD,
               twoD
             };
#if 1
       // Correct: This becomes a proper SgTypedefDeclaration in the AST
          typedef enum enumType2
             {
               zeroB = 0,
               oneB,
               twoB
             } UsingThisTypeCausesAnError;

       // Correct: This becomes a proper SgTypedefDeclaration in the AST
          typedef enum enumType3
             {
               zeroC = 0,
               oneC,
               twoC
             } enumType3;

          Array(enumType1 x);
          Array(enumType2 x);  // same as Array(UsingThisTypeCausesAnError x); (can't declare both)
          Array(ArrayB::enumType1 x);
          Array(::enumType1 x);
#endif
#if 1
          enumType1 & foo(enumType1 x);
          enumType2 & foo(enumType2 x);
          ::enumType1 & foo(::enumType1 x = ::zeroA);

       // This causes the same error as enumType2
       // Array(UsingThisTypeCausesAnError x);
#endif
   };

#if 1
void foo()
   {
     Array x(Array::zeroA);
  // Error: SgTypedef names are not properly qualified in function parameters
     Array y(Array::zeroB);
  // Array z(::zeroA);

     int zeroA;

     int a = x.foo(Array::zeroA);
     int b = x.foo(Array::zeroB);

  // DQ (6/10/2007): This is a bug in the hidden list computations at present
     int c = x.foo(::zeroA);
   }
#endif
