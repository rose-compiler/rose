#define DEMONSTRATE_BUG 1

class intArray;

class intArray_Descriptor_Type
   {
     public:
#if DEMONSTRATE_BUG
          friend void transpose( intArray & X );
#endif
//        void transpose( intArray & X );
   };

class intArray 
   {
     public:
         intArray_Descriptor_Type Array_Descriptor;
//       friend void transpose( intArray & X );
   };

void transpose (intArray & X )
// void intArray_Descriptor_Type::transpose (intArray & X )
   {
      intArray & Result = X; // *(new intArray());
   }

// This case works fine.  The code above demonstrates 
// the bug (suynthesized from 50K of A++ source code!!!)
// typedef long unsigned int size_t;

// This approach is independent of 32 or 64 bit systems (at least for GNU)
typedef __SIZE_TYPE__ size_t;

#if 0
// This is not defined by default for GNU unless we include the system headers.
// It is defined by default for ROSE, since it is required for the Plum Hall 
// tests and I think it is required within the C++ standard.  It is OK to
// redeclare it as long as it is done consistantly!
#ifdef __LP64__
// 64 bit case
typedef long unsigned int size_t;
#else
// 32 bit case
typedef unsigned int size_t;
#endif

class A
   { 
     public: 
          A(int) {};
          void *operator new (size_t Size) {};
   };

// A & x = *(new A(i));

void foo ()
   {
     A & Result = *(new A (1));

  // int j;

   }
#endif
